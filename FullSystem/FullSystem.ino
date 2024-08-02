/*
  TODO:
  - Set loggerID value
  - Set observation names (Digital Input names)
  - Implement thresholds
  - Implement text messages and such
*/

#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include "config.h"
#include "wellpro.h"
#include "mqtt_publisher.h"

// Define the config variables
const char* type = "Pump_Station_1";
const char* loggerID = "Some_ID_TBD";
const char* observationNames[] = {
  "Observation_Name_1",
  "Observation_Name_2",
  "Observation_Name_3",
  "Observation_Name_4",
  "Observation_Name_5",
  "Observation_Name_6",
  "Observation_Name_7",
  "Observation_Name_8"
};
float digitalInput[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

ModbusMaster node;  // Modbus Master instance

void setup() {
  pinMode(MODEM_EN_PIN, OUTPUT);
  digitalWrite(MODEM_EN_PIN, HIGH);
  Serial.begin(WELLPRO_BAUD_RATE);
  Serial2.begin(MODEM_BAUD_RATE, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  node.begin(WELLPRO_SLAVE_ID, Serial);
  delay(1000);
}

void loop() {
  bool readSuccess = readWellPro();
  if (readSuccess)
    publishMessage();
  delay(5000);
}
