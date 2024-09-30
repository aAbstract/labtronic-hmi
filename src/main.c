#include "lvgl/lvgl.h"
#include <stdio.h>

#include "modules/lt_ht107/lt_ht107.h"
#include "platforms/linux_sdl.h"
#include "platforms/linux_socket.h"

int main() {
  lv_init();
  unix_hal_init();
  if (linux_socket_init() != 0)
    return 1;

  lt_ht107_main_screen_create();

  while (1) {
    lv_timer_handler();
    linux_socket_loop();
    usleep(5000);
  }
  return 0;
}