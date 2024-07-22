#include "lvgl/lvgl.h"
#include <pthread.h>
#include <stdio.h>

#include "lt_ht113/lt_ht113.h"
#include "platforms/linux_sdl.h"
#include "platforms/linux_socket.h"

pthread_t emu_spi_thread;

void handle_emu_spi_device_msg(uint8_t msg_type, uint8_t* msg_value_buffer) {
  printf("Msg Type: %d\n", msg_type);
  float decoded_value = 0;
  ((uint8_t*)&decoded_value)[0] = msg_value_buffer[0];
  ((uint8_t*)&decoded_value)[1] = msg_value_buffer[1];
  ((uint8_t*)&decoded_value)[2] = msg_value_buffer[2];
  ((uint8_t*)&decoded_value)[3] = msg_value_buffer[3];
  printf("Msg Value: %0.3f\n", decoded_value);
}

int main() {
  lv_init();
  unix_hal_init();
  pthread_create(&emu_spi_thread, NULL, server_thread, NULL);

  lt_ht113_main_screen_create();

  while (1) {
    lv_timer_handler();
    usleep(5000);
  }
  return 0;
}