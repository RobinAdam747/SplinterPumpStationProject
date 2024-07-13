#include <ModbusMaster.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h> 

// Modbus settings
#define RS485Serial Serial2
ModbusMaster node;

// MQTT settings
const char* mqtt_server = "";
const char* mqtt_topic = "";

// Modem settings
SoftwareSerial modemSerial(16, 17);  // RX, TX 
const char* modem_server = "";
const int modem_port = 1883;
const char* modem_clientId = "";
const char* modem_username = "user";
const char* modem_password = "password";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

void setup() {
  Serial.begin(115200);
  RS485Serial.begin(9600);  // Set RS485 baud rate

  node.begin(1, RS485Serial);
  
  // Modem setup
  modemSerial.begin(9600);
  client.setServer(modem_server, modem_port);

  connectModem();
}

void loop() {
  // Handle Modbus RTU communication
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void connectModem() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker via modem...");
    if (client.connect(modem_clientId, modem_username, modem_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection via modem...");
    if (client.connect(modem_clientId, modem_username, modem_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void readModbusData() {
  uint8_t result = node.readHoldingRegisters(0, 10);

  if (result == node.ku8MBSuccess) {
    // Process received data
    uint16_t data = node.getResponseBuffer(0);
    
    // Publish data via MQTT
    char message[10]; 
    sprintf(message, "%d", data);
    client.publish(mqtt_topic, message);
  } else {
    Serial.print("Modbus read failed: ");
    Serial.println(result);
  }
}
