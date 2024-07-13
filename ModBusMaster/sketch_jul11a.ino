#include <ModbusMaster.h>

ModbusMaster node;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  
  // Initialize Modbus communication baud rate
  Serial2.begin(9600, SERIAL_8E1, 16, 17); // Change RX and TX pin numbers as needed
  
  // Initialize Modbus communication settings
  node.begin(1, Serial2); // Change the address as needed
}

void loop() {
  uint8_t result;
  uint16_t data[6];

  // Read 6 holding registers starting from address 0
  result = node.readInputRegisters(0, 6);

  if (result == node.ku8MBSuccess) {
    for (uint8_t i = 0; i < 6; i++) {
      data[i] = node.getResponseBuffer(i);
      Serial.print("Register ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(data[i]);
    }
  } else {
    Serial.print("Modbus Read Error: ");
    Serial.println(result);
  }

  delay(1000);
}
