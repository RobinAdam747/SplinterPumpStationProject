// mqtt_publisher.cpp
#include "mqtt_publisher.h"
#include "config.h"

extern HardwareSerial Serial2;

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
  serializeJson(doc, jsonBuffer);
  Serial2.print(jsonBuffer);
}

String fetchTimestamp() {
  // Send AT command to get the current timestamp
  Serial2.print("+++a");
  Serial2.write(13);  // Send Carriage Return (CR)
  delay(100);         // Wait for the modem to respond
  Serial2.print("AT+CCLK?");
  Serial2.write(13);  // Send Carriage Return (CR)
  delay(100);         // Wait for the modem to respond+++a

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

    // Convert the modem timestamp format to ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
    // Modem timestamp format: "yy/MM/dd,HH:mm:ss±zz"
    String year = "20" + timestamp.substring(0, 2);
    String month = timestamp.substring(3, 5);
    String day = timestamp.substring(6, 8);
    String time = timestamp.substring(9, 17);
    return year + "-" + month + "-" + day + "T" + time + "Z";
  }

  return "Timestamp not available";
}
