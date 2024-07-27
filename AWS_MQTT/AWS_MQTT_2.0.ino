#include <ModbusMaster.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include "secrets.h"

#define RXD2 16
#define TXD2 17

float DI[8];
double temperature, humid;

// Hardware serial for the modem (Serial1)
HardwareSerial& modem = Serial2;

ModbusMaster node;

const char* mqtt_server = "cloudmqtt.usr.cn";
const int mqtt_port = 1883;
const char* mqtt_user = "admin";
const char* mqtt_password = "admin";
const char* mqtt_topic = "esp32/pub";
const char* mqtt_subscribe_topic = "esp32/sub";

void connectModem() {
  // Example AT commands
  String responsecheck = sendATCommand("AT"); // Check if the modem is responding

  // Configure NTP Server
  sendATCommand("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"");

  // Check NTP time
  sendATCommand("AT+CIPSNTPTIME?");

  // Configure MQTT User
  sendATCommand("AT+MQTTUSERCFG=0,4,\"publisher\",\"admin\",\"admin\",0,0,\"\"");

  // Configure MQTT Connection
  sendATCommand("AT+MQTTCONNCFG=0,0,0,\"lwtt\",\"lwtm\",0,0");

  // Connect to MQTT Broker
  sendATCommand("AT+MQTTCONN=0,\"" + String(AWS_IOT_ENDPOINT) + "\",1883,1");

  // sendATCommand("AT+CMQTTSSLCFG=0,1,\"" + String(AWS_CERT_CA) + "\"");
  // sendATCommand("AT+CMQTTSSLCFG=0,2,\"" + String(AWS_CERT_CRT) + "\"");
  // sendATCommand("AT+CMQTTSSLCFG=0,3,\"" + String(AWS_CERT_PRIVATE) + "\"");
  
  if (responsecheck == "OK") {
    Serial.println("Modem connected and MQTT client initialized.");
  }
  delay(500);
}

bool isConnectedToMQTT() {
  String response = sendATCommand("AT+MQTTSTATUS");
  return response.indexOf("CONNECTED") != -1;
}

void connectAWS() {
  if (!isConnectedToMQTT()) {
    connectModem();
  }

  Serial.println("Connected to MQTT broker.");
}

void ensureAWSConnection() {
  if (!isConnectedToMQTT()) {
    Serial.println("Reconnecting to MQTT broker");
    connectAWS();
  }
}

void publishMessage() {
  StaticJsonDocument<200> doc;
  // doc["DI_01"] = DI[0];
  // doc["DI_02"] = DI[1];
  // doc["DI_03"] = DI[2];
  // doc["DI_04"] = DI[3];
  // doc["DI_05"] = DI[4];
  // doc["DI_06"] = DI[5];
  // doc["DI_07"] = DI[6];
  // doc["DI_08"] = DI[7];
  doc["time"] = sendATCommand("AT+CIPSNTPTIME?");
  doc["temperature"] = temperature;
  doc["humidity"] = humid;
  doc["device_id"] = "ESP32";

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);  // Serialize JSON document to buffer

  // Publish a message
  sendATCommand("AT+MQTTPUB=0,\"esp32/sub\",\""+ String(jsonBuffer) + "\",1,0");

  // Clean MQTT sessions
  sendATCommand("AT+MQTTCLEAN=0");

  Serial.println("Message published to MQTT topic.");
}

void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor
  modem.begin(9600, SERIAL_8N1, RXD2, TXD2); // Modem communication
  node.begin(1, modem);  // Modbus slave ID 1 for XY-MD02
  connectAWS();          // Connect to AWS IoT via MQTT
  delay(2000);
}

void loop() {
  // for (uint8_t i = 0; i < 8; ++i) {
  //   Serial.printf("DI_%02d: %f\n", i + 1, DI[i]);  // Format and print the string
  // }
  readdata();
  publishMessage();  // Publish sensor data

  ensureAWSConnection();  // Ensure MQTT connection is maintained
  delay(1000);            // Wait for 1 second
}

String sendATCommand(String command) {
  modem.println(command);
  delay(1000);
  
  String response = "";
  while (modem.available()) {
    response += (char)modem.read();
  }
  
  Serial.println("AT Command: " + command);
  Serial.println("Response: " + response);
  
  return response;
}

void readdata() {
  uint8_t result = node.readInputRegisters(0x0001, 1); // Read one register
  if (result == node.ku8MBSuccess) {
    temperature = node.getResponseBuffer(0) / 10.0; // First buffer index holds the temperature
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println(" C");
  }

  uint8_t result2 = node.readInputRegisters(0x0002, 1);
  if (result2 == node.ku8MBSuccess) {
    humid = node.getResponseBuffer(0) / 10.0;
    Serial.print("Humidity: ");
    Serial.print(humid);
    Serial.println(" %");
  }
  
  delay(1000);
}