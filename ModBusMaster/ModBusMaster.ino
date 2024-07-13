// https://blog.csdn.net/weixin_49763136/article/details/138808810
#include <ModbusMaster.h> 
#include <HardwareSerial.h>

ModbusMaster node;
HardwareSerial MySerial(2);
// uint16_t write_send[2] = {0x005A, 0x00B2}; 
uint8_t result;
uint16_t data[8];

void setup() 
{
  Serial.begin(19200);
  
  MySerial.begin(9600, SERIAL_8N2, 16, 17); // RX 16, TX 17
 
  node.begin(1, MySerial); // Slave ID as 1
  delay(1000);
}

void loop()
{
  result = node.readInputRegisters(0x0001, 8); 
  if (result == node.ku8MBSuccess) {
    Serial.println("Read successful:");
    for (int i = 0; i < 8; i++) 
    {
      data[i] = node.getResponseBuffer(i);
      Serial.print("Register ");
      Serial.print(i);
      Serial.print(" = ");
      Serial.println(data[i], HEX);
    }
  } else {
    Serial.print("Failed to read registers. Error: ");
    Serial.println(result);
  }
  delay(500);

  // for (int i=0; i<2; i++) 
  // {
  //   node.setTransmitBuffer(i, write_send[i]);
  // }
  // node.writeMultipleRegisters(0x00, 2);
}
