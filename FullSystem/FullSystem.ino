/*
  TODO:
  - Set loggerID value
  - Set observation names (Digital Input names)
  - Implement modem stuff (right now, MQTT is just handled by the ESP)
  - Add timestamp fetching for the JSON 
  - Implement thresholds
  - Implement text messages and such
  - Implement Voltage line sensor
  - Implement 3 colored LED light indicating thresholds etc
  - Other small details
*/

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <ModbusMaster.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Modbus Slave ID and communication settings
#define SLAVE_ID 1
#define SERIAL_BAUD_RATE 115200
#define MODEM_BAUD_RATE 9600
#define JSON_SIZE 2048

ModbusMaster node;  // Modbus Master instance

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// MQTT Configuration
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
float digitalInput[8];  // Digital Input data array

String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  connectWiFi();
  connectMQTT();
  connectNTP();
  Serial2.begin(MODEM_BAUD_RATE, SERIAL_8N2, 16, 17);  // RX 16, TX 17
  node.begin(SLAVE_ID, Serial2);
  delay(1000);
}

void loop() {
  readWellPro();
  publishMessage();
  ensureMQTTConnection();
  client.loop();
  delay(1000);
}

// Connect to Wi-Fi
void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wi-Fi connected");
}

// Connect to MQTT broker
void connectMQTT() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(messageHandler);
  Serial.println("Connecting to MQTT broker");
  while (!client.connect("ESP32Client", MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(".");
    delay(100);
  }
  if (client.connected()) {
    client.subscribe(MQTT_SUBSCRIBE_TOPIC);
    Serial.println("MQTT broker connected!");
  } else {
    Serial.println("MQTT broker connection failed!");
  }
}

void connectNTP() {
  timeClient.begin();
  timeClient.setTimeOffset(7200); // GMT+2
}

// Ensure MQTT connection
void ensureMQTTConnection() {
  if (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker");
    connectMQTT();
  }
}

// Publish sensor data
void publishMessage() {
  char jsonBuffer[JSON_SIZE];
  StaticJsonDocument<JSON_SIZE> doc;
  // Define the JSON structure
  doc["type"] = type;

  JsonObject properties = doc.createNestedObject("properties");
  properties["loggerID"] = loggerID;

  JsonArray observationNamesArray = properties.createNestedArray("observationNames");
  for (const char* name : observationNames) {
    observationNamesArray.add(name);
  }

  JsonObject observations = properties.createNestedObject("observations");
  String timestamp = "2023-08-08T13:55:00Z";  // Replace with fetch timestamp from modem
  JsonArray observationValues = observations.createNestedArray(timestamp);
  for (const float di : digitalInput) {
    observationValues.add(di);
  }

  serializeJson(doc, jsonBuffer);
  client.publish(MQTT_PUBLISH_TOPIC, jsonBuffer);
}

// Handle incoming messages
void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("Incoming message on topic: ");
  Serial.println(topic);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char* message = doc["message"];
  Serial.println(message);
}

// Read data from Well Pro sensor
void readWellPro() {
  uint8_t result = node.readInputRegisters(0x0001, 8);
  if (result == node.ku8MBSuccess) {
    Serial.println("Read successful:");
    for (int i = 0; i < 8; i++) {
      digitalInput[i] = node.getResponseBuffer(i);
      Serial.printf("DI_%02d: %f\n", i + 1, digitalInput[i]);
    }
  } else {
    Serial.print("Failed to read registers. Error: ");
    Serial.println(result);
  }
}

String getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  
  String timestamp = dayStamp + " " + timeStamp;
  Serial.println(timestamp);
  delay(1000);
}
