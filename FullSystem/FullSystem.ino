/*
  TODO:
  - Set loggerID value
  - Set observation names (Digital Input names)
  - Add timestamp fetching for the JSON
  - Implement thresholds
  - Implement text messages and such
  - Other small details
*/

#include <ArduinoJson.h>
#include <ModbusMaster.h>

// Modbus Slave ID and communication settings
#define SLAVE_ID 1
#define SERIAL_BAUD_RATE 115200
#define MODEM_BAUD_RATE 9600
#define RS485_RX_PIN 16
#define RS485_TX_PIN 17
#define JSON_SIZE 2048

ModbusMaster node;  // Modbus Master instance

// JSON Configuration
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
float digitalInput[8] = {0,0,0,0,0,0,0,0};  // Digital Input data array

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial2.begin(MODEM_BAUD_RATE, SERIAL_8N2, RS485_RX_PIN, RS485_TX_PIN);
  node.begin(SLAVE_ID, Serial2);
  delay(1000);
}

void loop() {
  readWellPro();
  publishMessage();
  delay(1000);
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
  String timestamp = fetchTimestamp();
  JsonArray observationValues = observations.createNestedArray(timestamp);
  for (const float di : digitalInput) {
    observationValues.add(di);
  }
  Serial.println(timestamp);
  serializeJson(doc, jsonBuffer);
  // Enable transmit mode
  //Serial2.print(jsonBuffer);
  // Print to Serial Monitor for debugging
  //Serial.println("Sent JSON: " + String(jsonBuffer));
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

String fetchTimestamp() {
  // Send AT command to get the current timestamp
  Serial2.print("+++a");
  Serial2.write(13);  // Send Carriage Return (CR)
  delay(100);  // Wait for the modem to respond
  Serial2.print("AT+CCLK?");
  Serial2.write(13);  // Send Carriage Return (CR)
  delay(100);  // Wait for the modem to respond+++a

  // Read the response
  String response = "";
  while (Serial2.available()) {
    char c = Serial2.read();
    response += c;
  }

  // Find the timestamp in the response
  int startIndex = response.indexOf("\"");
  int endIndex = response.indexOf("\"", startIndex + 1);

  if (startIndex != -1 && endIndex != -1) {
    String timestamp = response.substring(startIndex + 1, endIndex);

    // Convert the timestamp to the desired format if necessary
    return convertTimestampFormat(timestamp);
  }

  return "Timestamp not available";
}

String convertTimestampFormat(String timestamp) {
  // Convert the modem timestamp format to ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
  // Modem timestamp format: "yy/MM/dd,HH:mm:ss±zz"

  String year = "20" + timestamp.substring(0, 2);
  String month = timestamp.substring(3, 5);
  String day = timestamp.substring(6, 8);
  String time = timestamp.substring(9, 17);

  String isoTimestamp = year + "-" + month + "-" + day + "T" + time + "Z";
  return isoTimestamp;
}