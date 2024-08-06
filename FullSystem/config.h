// config.h

#ifndef CONFIG_H
#define CONFIG_H

// Modbus Slave ID and communication settings
#define WELLPRO_SLAVE_ID 1
#define WELLPRO_BAUD_RATE 9600
#define MODEM_BAUD_RATE 115200
#define MODEM_RX_PIN 16
#define MODEM_TX_PIN 17
#define MODEM_EN_PIN 5
#define JSON_SIZE 2048

// JSON Configuration
extern const char* type;
extern const char* loggerID;
extern const char* observationNames[8];

// Digital Input data array
extern float digitalInput[8];

// Modem HTTP Request
extern const char* modemAPN;
extern const char* url;
extern const char* token;
extern const char* contentType;
extern const char* jsonData;

extern const float threshold[8];


#endif // CONFIG_H
