// mqtt_publisher.cpp
#include "mqtt_publisher.h"
#include "config.h"

void publishMessage() {
  // Set modem to MQTT mode
  Serial2.println("usr.cn#AT+MODE=MQTT");
  delay(300);

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
  delay(1000);
  Serial2.print(jsonBuffer);
}

String fetchTimestamp() {
  // Clear any previous data in the serial buffer
  while (Serial2.available()) {
    Serial2.read();
  }
  Serial2.println("usr.cn#AT+CCLK\r");  // Send AT command to get the current timestamp
  delay(100);
  // Read the response
  String response = "";
  // long int time = millis();
  // while ((time + 500) > millis()) {
  //   while (Serial2.available()) {
  //     char c = Serial2.read();
  //     response += c;
  //   }
  // }
  response.trim();
  // Find the timestamp in the response
  int index = response.indexOf("+CCLK: ");
  if (index != -1) {
    int startIndex = response.indexOf("\"", index);
    int endIndex = response.indexOf("\"", startIndex + 1);

    //Serial2.print(response);
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
  }

  return response;
}
