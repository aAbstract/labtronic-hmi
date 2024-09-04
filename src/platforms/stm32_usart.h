#include "usart.h"
#include <stdint.h>

#include "../ltd_drivers/lt_ht113.h"

#define PACKET_HEADER_SIZE 3

extern UART_HandleTypeDef huart1;
uint8_t rx_buffer[32];
uint8_t is_packet_receiving = 0;

void init_stm32_usart() {
  init_ltd_driver(LT_HT113_PROTOCOL_VERSION, lt_ht113_driver_config, LT_HT113_DRIVER_CONFIG_SIZE);
  HAL_UART_Receive_IT(&huart1, rx_buffer, PACKET_HEADER_SIZE);
}

void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer);
void _handle_packet() {
  DeviceMsg device_msg;
  decode_packet(rx_buffer, &device_msg);
  handle_device_msg(device_msg.config.msg_type, device_msg.config.cfg2, device_msg.msg_value_buffer);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  if (huart != &huart1)
    return;

  // handle packet header case
  uint8_t packet_body_size = rx_buffer[2] - PACKET_HEADER_SIZE;
  if (
      !is_packet_receiving &&
      rx_buffer[0] == ((uint8_t*)&protocol_version)[0] &&
      rx_buffer[1] == ((uint8_t*)&protocol_version)[1] &&
      packet_body_size > 0) {
    is_packet_receiving = 1;
    HAL_UART_Receive_IT(&huart1, rx_buffer + PACKET_HEADER_SIZE, packet_body_size);
    return;
  }

  // handle packet body case
  if (is_packet_receiving) {
    is_packet_receiving = 0;
    HAL_UART_Receive_IT(&huart1, rx_buffer, PACKET_HEADER_SIZE);
    _handle_packet();
    return;
  }

  HAL_UART_Receive_IT(&huart1, rx_buffer, PACKET_HEADER_SIZE);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
  if (huart != &huart1)
    return;

  HAL_UART_Receive_IT(&huart1, rx_buffer, PACKET_HEADER_SIZE);
}