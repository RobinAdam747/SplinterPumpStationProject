#ifndef CONFIG_H
#define CONFIG_H

// Modbus Slave ID and communication settings
#define WELLPRO_SLAVE_ID 1
#define WELLPRO_BAUD_RATE 9600
#define MODEM_BAUD_RATE 115200
#define RS485_RX_PIN 16
#define RS485_TX_PIN 17
#define JSON_SIZE 2048

// JSON Configuration
extern const char* type;
extern const char* loggerID;
extern const char* observationNames[8];

// Digital Input data array
extern float digitalInput[8];

#endif  // CONFIG_H
