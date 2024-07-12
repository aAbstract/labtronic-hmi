#include "lvgl/lvgl.h"
#include <stdio.h>

#include "lt_ht107/lt_ht107.h"
#include "platforms/unix.h"

int main() {
  lv_init();
  unix_hal_init();

  lt_ht107_main_screen_create();

  while (1) {
    lv_timer_handler();
    usleep(5000);
  }
  return 0;
}