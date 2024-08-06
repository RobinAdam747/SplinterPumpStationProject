/*
  TODO:
  - Set loggerID value
  - Set observation names (Digital Input names)
*/

#include <ArduinoJson.h>
#include <ModbusMaster.h>
#include "config.h"
#include "wellpro.h"
#include "mqtt_publisher.h"
#include "modem_http.h"

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
const char* modemAPN = "your_modem_apn";
const char* url = "https://graph.facebook.com/v18.0/160064393865630/messages";
const char* token = "Bearer EAAMK9HvO62QBOy45ZCaErS2EivpUF3gZAFevfgb4EfNkZABNYp6ogmNPdMARai6HYgE2pRojL6TZAZCIyZC9EHxglc3mvOxCbN9lbHABv88oATTzA1olLpgYZBcncvB5g2sXZAtKeQyxn2UfZCM90y2YePh3uBFVL3wUMms9ZCXYsWmYtIUjmpbHpvKbrdt25wJLX9";
const char* contentType = "application/json";
const char* jsonData = "{ \"messaging_product\": \"whatsapp\", \"to\": \"27730860608\", \"type\": \"template\", \"template\": { \"name\": \"gateway_notifications\", \"language\": { \"code\": \"en\" } } }";

ModbusMaster node;  // Modbus Master instance

// Thresholds
const float threshold[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

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
  checkThresholds();
  delay(5000);
}
