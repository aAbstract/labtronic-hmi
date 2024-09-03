#include "../ltd_drivers/lt_ht113.h"

void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer);
void handle_packet(const uint8_t* packet) {
  DeviceMsg device_msg;
  decode_packet(packet, &device_msg);
  handle_device_msg(device_msg.config.msg_type, device_msg.config.cfg2, device_msg.msg_value_buffer);
}

// extern UART_HandleTypeDef huart1;
// uint8_t rx_buffer[32];
// void stm32_usart_task() {
//   if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
//     HAL_UART_Receive(&huart1, &rx_buffer, sizeof(rx_buffer), 500);
//     handle_packet(rx_buffer);
//   }
// }

extern UART_HandleTypeDef huart1;
uint8_t temp_chr;
uint8_t _rx_buffer[32];
uint8_t _rx_buffer_ptr = 0;
void init_stm32_usart() {
  HAL_UART_Receive_IT(&huart1, &temp_chr, 1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* _huart) {
  _rx_buffer[_rx_buffer_ptr++] = temp_chr;
  if (_rx_buffer[_rx_buffer_ptr - 2] == '\r' && _rx_buffer[_rx_buffer_ptr - 1] == '\n') {
    _rx_buffer_ptr = 0;
    handle_packet(_rx_buffer);
  }
  HAL_UART_Receive_IT(&huart1, &temp_chr, 1);
}