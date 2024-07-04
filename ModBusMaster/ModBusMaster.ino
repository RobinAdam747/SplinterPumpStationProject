#include <ModbusMaster.h>

#define RXuart 16  // RX pin connected to WellPro
#define TXuart 17  // TX pin connected to WellPro
#define UARTno 2   // UART number

ModbusMaster node;
HardwareSerial modbusSerial(UARTno);

void setup() {
  // Variables:
  int slaveID = 1;    // ID number of the slave device for ModBus comms (WellPro device)

  Serial.begin(115200);
  modbusSerial.begin(9600, SERIAL_8N1, RXuart, TXuart);  // RX = GPIO16, TX = GPIO17
  // Serial2.begin(9600, SERIAL_8N1, RXuart, TXuart);
  node.begin(slaveID, modbusSerial);                           // Slave ID 1
  Serial.println("Modbus communication setup complete.");
}

void loop() {
  uint8_t result;
  uint16_t data[10];

  // Ask WellPro for data
  Serial.println("Requesting information from digital inputs");
  uint16_t request = 0x01020000000879CC;
  // node.send(request);
  uint8_t writeSuccessFlag = node.writeSingleRegister(0x01, request);
  Serial.println("Write Success: " + writeSuccessFlag);

  Serial.println("Attempting to read registers...");
  result = node.readHoldingRegisters(0x01, 10);
  // result = node.readDiscreteInputs(0, 10);
  // Serial.println("Result: " + result);    // Print received data
  if (result == node.ku8MBSuccess) {
    Serial.println("Read successful:");
    for (int i = 0; i < 10; i++) {
      data[i] = node.getResponseBuffer(i);
      Serial.print("Register ");
      Serial.print(i);
      Serial.print(" = ");
      Serial.println(data[i]);
    }
  } else {
    Serial.print("Failed to read registers. Error: ");
    Serial.println(result);
  }
  delay(1000);
}
