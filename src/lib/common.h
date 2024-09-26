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

#define LT_HMI_NAN -9999

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
  lv_obj_align(div_label, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_style_text_font(div_label, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_style_text_color(div_label, div_color, LV_PART_MAIN);
  lv_label_set_text_fmt(div_label, "%s    Div Example Text", LV_SYMBOL_STOP);
}

lv_obj_t* device_title;
void set_device_title(const char* device_title_str) {
  device_title = lv_label_create(lv_scr_act());
  lv_obj_align(device_title, LV_ALIGN_TOP_MID, 0, TOP_BAR_HEIGHT + 24);
  lv_obj_set_style_text_color(device_title, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_text_font(device_title, &lv_font_montserrat_30, LV_PART_MAIN);
  lv_label_set_text(device_title, device_title_str);
}

typedef struct LVErrorToasts {
  lv_obj_t* toasts_cont;
  lv_obj_t* toasts_label;
} LVErrorToasts;
LVErrorToasts lv_error_toasts;
const char* error_toasts_buffer[4] = {0x00};
uint8_t error_toasts_buffer_write_ptr = 0;
lv_timer_t* error_toasts_timer = NULL;

void error_toasts_timer_handler(lv_timer_t* _timer) {
  lv_obj_add_flag(lv_error_toasts.toasts_cont, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(device_title, LV_OBJ_FLAG_HIDDEN);
  uint8_t error_toasts_buffer_size = sizeof(error_toasts_buffer) / sizeof(error_toasts_buffer[0]);
  for (uint8_t i = 0; i < error_toasts_buffer_size; i++)
    error_toasts_buffer[i] = NULL;
  error_toasts_buffer_write_ptr = 0;
}

void create_error_toasts(uint16_t width) {
  lv_obj_t* toasts_cont = lv_obj_create(lv_scr_act());
  lv_obj_set_size(toasts_cont, width, LV_SIZE_CONTENT);
  lv_obj_set_pos(toasts_cont, 16, TOP_BAR_HEIGHT + 16);
  lv_obj_set_style_bg_color(toasts_cont, CHX_ERROR_COLOR, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(toasts_cont, LV_OPA_20, LV_PART_MAIN);
  lv_obj_set_style_radius(toasts_cont, 4, LV_PART_MAIN);
  lv_obj_set_style_border_width(toasts_cont, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(toasts_cont, CHX_ERROR_COLOR, LV_PART_MAIN);

  lv_obj_t* toasts_label = lv_label_create(toasts_cont);
  lv_obj_align(toasts_label, LV_ALIGN_LEFT_MID, 0, 0);
  lv_obj_set_style_text_font(toasts_label, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_obj_set_style_text_color(toasts_label, CHX_ERROR_COLOR, LV_PART_MAIN);

  lv_error_toasts.toasts_cont = toasts_cont;
  lv_error_toasts.toasts_label = toasts_label;
  lv_obj_add_flag(toasts_cont, LV_OBJ_FLAG_HIDDEN);

  error_toasts_timer = lv_timer_create(error_toasts_timer_handler, 5000, NULL);
}

void add_error_toast(const char* msg) {
  uint8_t error_toasts_buffer_size = sizeof(error_toasts_buffer) / sizeof(error_toasts_buffer[0]);
  error_toasts_buffer[error_toasts_buffer_write_ptr] = msg;
  error_toasts_buffer_write_ptr = (error_toasts_buffer_write_ptr + 1) % error_toasts_buffer_size;

  char render_buffer[256];
  uint8_t write_ptr = 0;
  for (uint8_t i = 0; i < error_toasts_buffer_size; i++) {
    if (error_toasts_buffer[i] == NULL)
      continue;
    sprintf(render_buffer + write_ptr, "%s    %s\n", LV_SYMBOL_CLOSE, error_toasts_buffer[i]);
    write_ptr += 8 + strlen(error_toasts_buffer[i]);
  }
  render_buffer[write_ptr - 1] = 0x00; // remove last LF

  lv_label_set_text(lv_error_toasts.toasts_label, render_buffer);
  lv_obj_clear_flag(lv_error_toasts.toasts_cont, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(device_title, LV_OBJ_FLAG_HIDDEN);
  if (error_toasts_timer != NULL)
    lv_timer_reset(error_toasts_timer);
}