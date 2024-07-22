#include "lvgl/lvgl.h"

#include "../common/common.h"
#include "lt_ht113_brass_slab.h"

void lt_ht113_main_screen_create() {
  add_lt_frame("LT-HT113");
  lv_obj_t* img = lv_img_create(lv_scr_act());
  LV_IMG_DECLARE(lt_ht113_brass_slab);
  lv_img_set_src(img, &lt_ht113_brass_slab);
  lv_obj_center(img);
}