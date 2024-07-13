// Code found at: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html and https://www.youtube.com/watch?v=TGusjcKSNIU

#define RXuart 16  // RX pin connected to WellPro
#define TXuart 17  // TX pin connected to WellPro

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXuart, TXuart);

  Serial.println("Setup complete");

  // UART setup
  // const uart_port_t uart_num = UART_NUM_2;
  // uart_config_t uart_config = {
  //   .baud_rate = 9600,
  //   .data_bits = UART_DATA_8_BITS,
  //   .parity = UART_PARITY_DISABLE,
  //   .stop_bits = UART_STOP_BITS_1,
  //   .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
  //   .rx_flow_ctrl_thresh = 122,
  // };

  // Configure UART parameters
  // ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("Transmitting request...");
  //Serial.write(69);
  Serial2.write(1);
  delay(1000);

  // Serial.println("Data Received: " + Serial.readString());

}
