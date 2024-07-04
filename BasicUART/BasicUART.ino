// Code found at: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html and https://www.youtube.com/watch?v=TGusjcKSNIU

// #define RXp2 16
// #define TXp2 17

void setup() {
  // put your setup code here, to run once:
  // Serial.begin(115200);
  Serial.begin(9600);

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

  // Serial.println("Data Received: " + Serial.readString());

}
