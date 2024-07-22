#include "lv_drivers/sdl/sdl.h"
#include <pthread.h>

#define MONITOR_HOR_RES 1280
#define MONITOR_VER_RES 800

void sdl_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

lv_disp_draw_buf_t disp_buf1;
lv_color_t buf1_1[MONITOR_HOR_RES * 100];
lv_color_t buf1_2[MONITOR_HOR_RES * 100];
lv_disp_drv_t disp_drv;
pthread_t thr_tick;
bool end_tick = false;

void* tick_thread() {
  while (!end_tick) {
    usleep(5000);
    lv_tick_inc(5);
  }
  return NULL;
}

lv_disp_t* unix_hal_init() {
  sdl_init();
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 100);
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = sdl_display_flush;
  disp_drv.hor_res = MONITOR_HOR_RES;
  disp_drv.ver_res = MONITOR_VER_RES;
  disp_drv.antialiasing = 1;
  lv_disp_t* disp = lv_disp_drv_register(&disp_drv);
  lv_theme_t* th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);
  end_tick = false;
  pthread_create(&thr_tick, NULL, tick_thread, NULL);
  return disp;
}