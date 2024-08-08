// modem_http.cpp

#include <Arduino.h>
#include "modem_http.h"
#include "config.h"

void initialiseModem() {

  // Set APN
  Serial2.print("usr.cn#AT+CGDCONT=1,\"IP\",\"");
  Serial2.print(modemAPN);
  Serial2.println("\"\r");
  delay(200);

  // Connect to network
  Serial2.println("usr.cn#AT+CGATT=1\r");
  delay(200);

  // Set up HTTP mode
  Serial2.println("usr.cn#AT+HTTPINIT\r");
  delay(200);
}

void sendHttpPost() {
  // Set HTTP parameters
  Serial2.println("usr.cn#AT+HTTPPARA=\"CID\",1\r");
  delay(200);

  Serial2.print("usr.cn#AT+HTTPPARA=\"URL\",\"");
  Serial2.print(url);
  Serial2.println("\"\r");
  delay(200);

  Serial2.print("usr.cn#AT+HTTPPARA=\"USERDATA\",\"Authorization: ");
  Serial2.print(token);
  Serial2.println("\"\r");
  delay(200);

  Serial2.print("usr.cn#AT+HTTPPARA=\"CONTENT\",\"");
  Serial2.print(contentType);
  Serial2.println("\"\\r");
  delay(200);

  // Set data length
  Serial2.print("usr.cn#AT+HTTPDATA=");
  Serial2.print(strlen(jsonData));
  Serial2.println(",10000\r");
  delay(200);

  // Send JSON data
  Serial2.print(jsonData);
  delay(200);

  // Start HTTP POST
  Serial2.println("usr.cn#AT+HTTPACTION=1\r");
  delay(200);

  // Clean up HTTP session
  // Terminate HTTP session
  Serial2.println("usr.cn#AT+HTTPTERM\r");
  delay(200);

  // Switch modem back to MQTT mode
  Serial2.println("usr.cn#AT+MODE=MQTT\r");
  delay(200);
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
