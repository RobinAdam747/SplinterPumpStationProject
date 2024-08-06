// modem_http.cpp

#include <Arduino.h>
#include "modem_http.h"
#include "config.h"

void initialiseModem() {

  // Set APN
  Serial2.print("usr.cn#AT+CGDCONT=1,\"IP\",\"");
  Serial2.print(modemAPN);
  Serial2.println("\"");
  if (!waitForResponse("OK")) return;

  // Connect to network
  Serial2.println("usr.cn#AT+CGATT=1");
  if (!waitForResponse("OK")) return;

  // Set up HTTP mode
  Serial2.println("usr.cn#AT+HTTPINIT");
  if (!waitForResponse("OK")) return;
}

void sendHttpPost() {
  // Set HTTP parameters
  Serial2.println("usr.cn#AT+HTTPPARA=\"CID\",1");
  waitForResponse("OK");

  Serial2.print("usr.cn#AT+HTTPPARA=\"URL\",\"");
  Serial2.print(url);
  Serial2.println("\"");
  waitForResponse("OK");

  Serial2.print("usr.cn#AT+HTTPPARA=\"USERDATA\",\"Authorization: ");
  Serial2.print(token);
  Serial2.println("\"");
  waitForResponse("OK");

  Serial2.print("usr.cn#AT+HTTPPARA=\"CONTENT\",\"");
  Serial2.print(contentType);
  Serial2.println("\"");
  waitForResponse("OK");

  // Set data length
  Serial2.print("usr.cn#AT+HTTPDATA=");
  Serial2.print(strlen(jsonData));
  Serial2.println(",10000");
  if (!waitForResponse("DOWNLOAD")) return;

  // Send JSON data
  Serial2.print(jsonData);
  if (!waitForResponse("OK")) return;

  // Start HTTP POST
  Serial2.println("usr.cn#AT+HTTPACTION=1");
  if (!waitForResponse("OK")) return;

  // Clean up HTTP session
  // Terminate HTTP session
  Serial2.println("usr.cn#AT+HTTPTERM");
  waitForResponse("OK");

  // Switch modem back to MQTT mode
  Serial2.println("usr.cn#AT+MODE=MQTT");
  waitForResponse("OK");
}

bool waitForResponse(const char* expectedResponse) {
  unsigned long timeout = millis() + 5000;  // 5 seconds timeout
  while (millis() < timeout) {
    if (Serial2.available()) {
      String response = Serial2.readString();
      if (response.indexOf(expectedResponse) != -1) {
        return true;
      }
    }
  }
  return false;
}


void checkThresholds() {
  for (int i = 0; i < 8; ++i) {
    if (digitalInput[i] > threshold[i]) {
      // Initialize the modem
      initialiseModem();
      // Send the HTTP POST request
      sendHttpPost();
    }
  }
}
