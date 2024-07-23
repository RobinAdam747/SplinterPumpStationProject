#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"

#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

float DI[8];

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

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

void connectAWS() {
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Create a message handler
  client.setCallback(messageHandler);

  Serial.println("Connecting to AWS IoT");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("AWS IoT Connected!");
}

void ensureAWSConnection() {
  if (!client.connected()) {
    Serial.println("Reconnecting to AWS IoT");
    connectAWS();
  }
}

void publishMessage() {
  StaticJsonDocument<200> doc;
  doc["DI_01"] = DI[0];
  doc["DI_02"] = DI[1];
  doc["DI_03"] = DI[2];
  doc["DI_04"] = DI[3];
  doc["DI_05"] = DI[4];
  doc["DI_06"] = DI[5];
  doc["DI_07"] = DI[6];
  doc["DI_08"] = DI[7];
  doc["device_id"] = "ESP32";

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);  // Serialize JSON document to buffer

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);  // Publish message to topic
}

void messageHandler(char* topic, byte* payload, unsigned int length) {
  Serial.print("incoming: ");
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

void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor
  connectWiFi();         // Connect to Wi-Fi
  connectAWS();          // Connect to AWS IoT
}

void loop() {
  for (uint8_t i = 0; i < 8; ++i) {
    // DI[i] = readFromWellPro();    // Uncomment and implement sensor reading function
    Serial.printf("DI_%02d: %f\n", i + 1, DI[i]);  // Format and print the string
  }

  publishMessage();  // Publish sensor data

  ensureAWSConnection();  // Ensure MQTT connection is maintained
  client.loop();          // Maintain MQTT connection
  delay(1000);            // Wait for 1 second
}
