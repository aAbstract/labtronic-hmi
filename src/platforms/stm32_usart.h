#include "usart.h"
#include <stdint.h>

// #include "../ltd_drivers/lt_ht107.h"
#include "../ltd_drivers/lt_ht113.h"

#define SCREEN_UART &huart1
#define RX_DMA_BUFFER_SIZE 256

extern UART_HandleTypeDef huart1;
uint8_t rx_buffer[RX_DMA_BUFFER_SIZE];
__attribute__((section(".ext_ram_d3"))) uint8_t rx_dma_buffer[RX_DMA_BUFFER_SIZE];

void init_stm32_usart() {
  // init_ltd_driver(LT_HT107_PROTOCOL_VERSION, lt_ht107_driver_config, LT_HT107_DRIVER_CONFIG_SIZE);
  init_ltd_driver(LT_HT113_PROTOCOL_VERSION, lt_ht113_driver_config, LT_HT113_DRIVER_CONFIG_SIZE);
  HAL_UARTEx_ReceiveToIdle_DMA(SCREEN_UART, rx_dma_buffer, RX_DMA_BUFFER_SIZE);
}

void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer);
void parse_rx_buffer_stream(uint8_t rx_buffer_size) {
  uint8_t packet_buffer[32];
  uint8_t write_ptr = 0;
  for (uint8_t i = 0; i < rx_buffer_size; i++) {
    packet_buffer[write_ptr] = rx_buffer[i];
    if (packet_buffer[write_ptr] != 0x0A) {
      write_ptr++;
      continue;
    }
    if (packet_buffer[write_ptr - 1] != 0x0D) {
      write_ptr++;
      continue;
    }
    if (packet_buffer[0] != ((uint8_t*)&protocol_version)[0] || packet_buffer[1] != ((uint8_t*)&protocol_version)[1]) {
      write_ptr = 0;
      continue;
    }
    DeviceMsg device_msg;
    if (decode_packet(packet_buffer, &device_msg) == RC_OK)
      handle_device_msg(device_msg.config.msg_type, device_msg.config.cfg2, device_msg.msg_value_buffer);
    write_ptr = 0;
  }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size) {
  if (huart != SCREEN_UART)
    return;

  memcpy(rx_buffer, rx_dma_buffer, RX_DMA_BUFFER_SIZE);
  parse_rx_buffer_stream(size);
  HAL_UARTEx_ReceiveToIdle_DMA(SCREEN_UART, rx_dma_buffer, RX_DMA_BUFFER_SIZE);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
  if (huart == SCREEN_UART)
    HAL_UARTEx_ReceiveToIdle_DMA(SCREEN_UART, rx_dma_buffer, RX_DMA_BUFFER_SIZE);
}
