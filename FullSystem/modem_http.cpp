// modem_http.cpp

#include <Arduino.h>
#include "modem_http.h"
#include "config.h"

void initialiseModem() {

  // Set APN
  Serial2.println("usr.cn#AT+CGDCONT=1,\"IP\",\"" + modemAPN + "\"\r");
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

  Serial2.println("usr.cn#AT+HTTPPARA=\"URL\",\""+url+"\"\r");
  delay(200);

  Serial2.println("usr.cn#AT+HTTPPARA=\"USERDATA\",\"Authorization: " + token + "\"\r");
  delay(200);

  Serial2.println("usr.cn#AT+HTTPPARA=\"CONTENT\",\"" + contentType + "\"\\r");
  delay(200);

  // Set data length
  Serial2.println("usr.cn#AT+HTTPDATA="+jsonData+",10000\r");
  delay(200);

  // Send JSON data
  Serial2.println(jsonData);
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
