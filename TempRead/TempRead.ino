#include <ModbusMaster.h>

#define RXD2 16
#define TXD2 17

ModbusMaster node;

void setup() {
  Serial.begin(19200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  node.begin(1, Serial2);  // Modbus slave ID 3 for XY-MD02
  delay(1000);
}

void loop() {
  uint8_t result = node.readInputRegisters(0x0001, 1); // Read one register
  if (result == node.ku8MBSuccess) {
    double temperature = node.getResponseBuffer(0) / 10.0; // First buffer index holds the temperature
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println(" C");
  }

  uint8_t result2 = node.readInputRegisters(0x0002, 1);
  if (result2 == node.ku8MBSuccess) {
    double temperature = node.getResponseBuffer(0) / 10.0;
    Serial.print("Humidity: ");
    Serial.print(temperature);
    Serial.println(" %");
  }
  
  delay(2000); // Delay between readings
}