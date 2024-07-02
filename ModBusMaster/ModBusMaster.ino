#include <ModbusMaster.h>
ModbusMaster node;
HardwareSerial modbusSerial(1);

void setup() {
    Serial.begin(115200);
    modbusSerial.begin(9600, SERIAL_8N1, 16, 17); // RX = GPIO16, TX = GPIO17
    node.begin(1, modbusSerial); // Slave ID 1
    Serial.println("Modbus communication setup complete.");
}

void loop() {
    uint8_t result;
    uint16_t data[10];
    Serial.println("Attempting to read registers...");
    result = node.readHoldingRegisters(0, 10);
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
