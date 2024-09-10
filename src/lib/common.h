#include "lvgl/lvgl.h"

#include "lt_logo.h"

#define LT_HMI_HOR_RES 1280
#define LT_HMI_VER_RES 800

#define TOP_BAR_HEIGHT 60

#define CHX_DARK_BG_COLOR lv_color_hex(0x0B0E1F)
#define CHX_ACCENT_COLOR lv_color_hex(0x29B2F8)
#define CHX_FONT_COLOR lv_color_hex(0x8BA2CC)
#define CHX_EMPTY_GAUGE_COLOR lv_color_hex(0x2D3A4B)
#define CHX_SUCCESS_COLOR lv_palette_main(LV_PALETTE_GREEN)
#define CHX_WARNING_COLOR lv_palette_main(LV_PALETTE_YELLOW)
#define CHX_ERROR_COLOR lv_palette_main(LV_PALETTE_RED)

#define DEVICE_MSG_SUCCESS 0
#define DEVICE_MSG_INFO 1
#define DEVICE_MSG_WARNING 2
#define DEVICE_MSG_ERROR 3

void create_lt_frame(const char* device_model) {
  lv_obj_set_style_bg_color(lv_scr_act(), CHX_DARK_BG_COLOR, LV_PART_MAIN);

  // top bar container
  lv_obj_t* top_bar_cont = lv_obj_create(lv_scr_act());
  lv_obj_set_size(top_bar_cont, LT_HMI_HOR_RES, TOP_BAR_HEIGHT);
  lv_obj_align(top_bar_cont, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_clear_flag(top_bar_cont, LV_OBJ_FLAG_SCROLL_ELASTIC);
  lv_obj_set_style_bg_color(top_bar_cont, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_radius(top_bar_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(top_bar_cont, 4, LV_PART_MAIN);
  lv_obj_set_style_border_color(top_bar_cont, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_border_side(top_bar_cont, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);

  // device tag
  lv_obj_t* device_tag_cont = lv_obj_create(lv_scr_act());
  lv_obj_set_size(device_tag_cont, 250, 60);
  lv_obj_align(device_tag_cont, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(device_tag_cont, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_radius(device_tag_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(device_tag_cont, 0, LV_PART_MAIN);
  lv_obj_t* device_model_label = lv_label_create(device_tag_cont);
  lv_obj_align(device_model_label, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_text_color(device_model_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_text_font(device_model_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_label_set_text(device_model_label, device_model);

  // lt logo
  lv_obj_t* lt_logo_img = lv_img_create(lv_scr_act());
  lv_obj_align(lt_logo_img, LV_ALIGN_TOP_RIGHT, 0, 8);
  decompress_lt_logo();
  lv_img_set_src(lt_logo_img, &lt_logo);
}

void style_neon_div(lv_color_t div_color, lv_obj_t* div_cont, lv_obj_t* div_label, uint16_t width, uint16_t height) {
  lv_obj_set_size(div_cont, width, height);
  lv_obj_set_style_bg_color(div_cont, div_color, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(div_cont, LV_OPA_20, LV_PART_MAIN);
  lv_obj_set_style_radius(div_cont, 4, LV_PART_MAIN);
  lv_obj_set_style_border_width(div_cont, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(div_cont, div_color, LV_PART_MAIN);
  lv_obj_align_to(div_label, NULL, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_style_text_font(div_label, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_style_text_color(div_label, div_color, LV_PART_MAIN);
  lv_label_set_text_fmt(div_label, "%s    Div Example Text", LV_SYMBOL_STOP);
}