#include "lt_ht107_model.h"

void lt_ht107_main_screen_create() {
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0B0E1F), LV_PART_MAIN);
  lv_obj_t* img = lv_img_create(lv_scr_act());
  LV_IMG_DECLARE(lt_ht107_model);
  lv_img_set_src(img, &lt_ht107_model);
  lv_obj_center(img);
}