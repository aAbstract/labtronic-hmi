#include "lvgl/lvgl.h"

#include "../../lib/common.h"
#include "lt_ht113_stainless_steel_cylinder_d20.h"

#define INNER_WINDOW_WIDTH 1000
#define INNER_WINDOW_HEIGHT 625
#define DEVICE_IMG_WIDTH 200
#define READINGS_PANEL_SIZE INNER_WINDOW_WIDTH - DEVICE_IMG_WIDTH
#define READINGS_PANEL_MARGIN 16

// spi params
#define T_SAM_MSG_TYPE 0
#define T_AMB_MSG_TYPE 1
#define T_REF_MSG_TYPE 2
#define W_FLW_MSG_TYPE 3
#define X_WLS_MSG_TYPE 4
#define DEVICE_ERROR_MSG_TYPE 14

const static char* t_sam_label_str = "Sample Temprature:";
const static char* t_amb_label_str = "Ambient Temprature:";
const static char* t_ref_label_str = "Reference Temprature:";

const static char* brass_cylinder_d20 = "Brass Cylinder D=20mm";
const static char* brass_cylinder_d30 = "Brass Cylinder D=30mm";
const static char* brass_slab = "Brass Slab";
const static char* brass_sphere = "Brass Sphere";
const static char* stainless_steel_cylinder_d20 = "Stainless Steel D=20mm";
const static char* stainless_steel_slab = "Stainless Steel Slab";
const static char* stainless_steel_sphere = "Stainless Steel Sphere";

lv_obj_t* inner_window;
void create_inner_window() {
  inner_window = lv_obj_create(lv_scr_act());
  lv_obj_set_size(inner_window, INNER_WINDOW_WIDTH, LV_SIZE_CONTENT);
  lv_obj_center(inner_window);
  lv_obj_set_style_bg_color(inner_window, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_pad_all(inner_window, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(inner_window, 0, LV_PART_MAIN);
}

lv_obj_t* device_img_cont;
void create_device_img() {
  device_img_cont = lv_obj_create(inner_window);
  lv_obj_set_size(device_img_cont, LV_SIZE_CONTENT, INNER_WINDOW_HEIGHT);
  lv_obj_align_to(device_img_cont, NULL, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_set_style_bg_color(device_img_cont, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_pad_all(device_img_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(device_img_cont, 0, LV_PART_MAIN);

  lv_obj_t* device_img = lv_img_create(device_img_cont);
  lv_obj_set_size(device_img, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_center(device_img);
  decompress_lt_ht113_stainless_steel_cylinder_d20();
  lv_img_set_src(device_img, &lt_ht113_stainless_steel_cylinder_d20);
}

lv_obj_t* readings_panel;
void create_readings_panel() {
  readings_panel = lv_obj_create(inner_window);
  lv_obj_set_size(readings_panel, READINGS_PANEL_SIZE, INNER_WINDOW_HEIGHT);
  lv_obj_align_to(readings_panel, NULL, LV_ALIGN_TOP_LEFT, DEVICE_IMG_WIDTH, 0);
  lv_obj_set_style_bg_color(readings_panel, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_border_width(readings_panel, 0, LV_PART_MAIN);
}

lv_obj_t* lt_ht113_sample_cont;
lv_obj_t* lt_ht113_sample_label;
void create_lt_ht113_sample() {
  lt_ht113_sample_cont = lv_obj_create(readings_panel);
  lt_ht113_sample_label = lv_label_create(lt_ht113_sample_cont);
  style_neon_div(CHX_ACCENT_COLOR, lt_ht113_sample_cont, lt_ht113_sample_label, READINGS_PANEL_SIZE - 48);
  lv_obj_align_to(lt_ht113_sample_cont, NULL, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, stainless_steel_cylinder_d20);
}

lv_obj_t* t_sam_cont;
lv_obj_t* t_sam_label;
void create_device_reading_t_sam() {
  t_sam_cont = lv_obj_create(readings_panel);
  t_sam_label = lv_label_create(t_sam_cont);
  style_neon_div(lv_color_hex(0x9C27B0), t_sam_cont, t_sam_label, READINGS_PANEL_SIZE - 48);
  lv_obj_set_style_bg_opa(t_sam_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_align_to(t_sam_cont, NULL, LV_ALIGN_LEFT_MID, 0, 0);
  lv_label_set_text_fmt(t_sam_label, "%s    Sample Temprature: -- C", LV_SYMBOL_RIGHT);
}

lv_obj_t* t_amb_cont;
lv_obj_t* t_amb_label;
void create_device_reading_t_amb() {
  t_amb_cont = lv_obj_create(readings_panel);
  t_amb_label = lv_label_create(t_amb_cont);
  style_neon_div(lv_color_hex(0x00BCD4), t_amb_cont, t_amb_label, READINGS_PANEL_SIZE - 48);
  lv_obj_set_style_bg_opa(t_amb_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_align_to(t_amb_cont, t_sam_cont, LV_ALIGN_OUT_TOP_LEFT, 0, -READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(t_amb_label, "%s    Ambient Temprature: -- C", LV_SYMBOL_RIGHT);
}

lv_obj_t* t_ref_cont;
lv_obj_t* t_ref_label;
void create_device_reading_t_ref() {
  t_ref_cont = lv_obj_create(readings_panel);
  t_ref_label = lv_label_create(t_ref_cont);
  style_neon_div(lv_color_hex(0x009688), t_ref_cont, t_ref_label, READINGS_PANEL_SIZE - 48);
  lv_obj_set_style_bg_opa(t_ref_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_align_to(t_ref_cont, t_sam_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(t_ref_label, "%s    Reference Temprature: -- C", LV_SYMBOL_RIGHT);
}

lv_obj_t* w_flw_cont;
lv_obj_t* w_flw_label;
void create_device_reading_w_flw() {
  w_flw_cont = lv_obj_create(readings_panel);
  w_flw_label = lv_label_create(w_flw_cont);
  style_neon_div(lv_color_hex(0x2196F3), w_flw_cont, w_flw_label, 300);
  lv_obj_center(w_flw_label);
  lv_obj_set_style_bg_opa(w_flw_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_set_style_text_font(w_flw_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_align_to(w_flw_cont, t_ref_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(w_flw_label, "Water Flow: --- L/m");
}

lv_obj_t* device_msg_cont;
lv_obj_t* device_msg_label;
void create_device_msg() {
  device_msg_cont = lv_obj_create(readings_panel);
  device_msg_label = lv_label_create(device_msg_cont);
  style_neon_div(CHX_WARNING_COLOR, device_msg_cont, device_msg_label, READINGS_PANEL_SIZE - 48);
  lv_obj_align_to(device_msg_cont, NULL, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_WARNING, "WATER LEVEL - INSUFFICIENT");
}

void lt_ht113_main_screen_create() {
  create_lt_frame("LT-HT113");

  create_inner_window();
  create_device_img();
  create_readings_panel();

  create_lt_ht113_sample();

  create_device_reading_t_sam();
  create_device_reading_t_amb();
  create_device_reading_t_ref();
  create_device_reading_w_flw();

  create_device_msg();
}

void set_x_wls(uint8_t wls_state) {
  if (wls_state == 0) {
    lv_obj_set_style_bg_color(device_msg_cont, CHX_WARNING_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_color(device_msg_cont, CHX_WARNING_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_color(device_msg_label, CHX_WARNING_COLOR, LV_PART_MAIN);
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_WARNING, "WATER LEVEL - INSUFFICIENT");
  } else {
    lv_obj_set_style_bg_color(device_msg_cont, CHX_SUCCESS_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_color(device_msg_cont, CHX_SUCCESS_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_color(device_msg_label, CHX_SUCCESS_COLOR, LV_PART_MAIN);
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_OK, "WATER LEVEL - OK");
  }
}
void set_device_error(uint8_t device_error) {
  lv_obj_set_style_bg_color(device_msg_cont, CHX_ERROR_COLOR, LV_PART_MAIN);
  lv_obj_set_style_border_color(device_msg_cont, CHX_ERROR_COLOR, LV_PART_MAIN);
  lv_obj_set_style_text_color(device_msg_label, CHX_ERROR_COLOR, LV_PART_MAIN);
  if (device_error == 0xF0)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Temperature {TC-S} OTP");
  else if (device_error == 0xF1)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Temperature {TC-A} OTP");
  else if (device_error == 0xF2)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Temperature {TC-R} OTP");
  else if (device_error == 0xF3)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Level Sensor out of range");
  else if (device_error == 0xFF)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Invalid Packet");
  else if (device_error == 0xC0)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Invalid Message Type Code");
  else if (device_error == 0xC1)
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Invalid LT-HT113 Sample Code");
  else
    lv_label_set_text_fmt(device_msg_label, "%s    %s", LV_SYMBOL_CLOSE, "Unknown Device Message Error Code");
}

float decode_float(uint8_t* data_buffer) {
  float decoded_value = 0;
  ((uint8_t*)&decoded_value)[0] = data_buffer[0];
  ((uint8_t*)&decoded_value)[1] = data_buffer[1];
  ((uint8_t*)&decoded_value)[2] = data_buffer[2];
  ((uint8_t*)&decoded_value)[3] = data_buffer[3];
  return decoded_value;
}
uint8_t cur_device_mode = 0x00;
void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer) {
  // handle LT_HT113 msg_type
  if (msg_type == T_SAM_MSG_TYPE)
    lv_label_set_text_fmt(t_sam_label, "%s    %s %d C", LV_SYMBOL_RIGHT, t_sam_label_str, (uint32_t)decode_float(msg_value_buffer));
  else if (msg_type == T_AMB_MSG_TYPE)
    lv_label_set_text_fmt(t_amb_label, "%s    %s %d C", LV_SYMBOL_RIGHT, t_amb_label_str, (uint32_t)decode_float(msg_value_buffer));
  else if (msg_type == T_REF_MSG_TYPE)
    lv_label_set_text_fmt(t_ref_label, "%s    %s %d C", LV_SYMBOL_RIGHT, t_ref_label_str, (uint32_t)decode_float(msg_value_buffer));
  else if (msg_type == W_FLW_MSG_TYPE)
    lv_label_set_text_fmt(w_flw_label, "Water Flow: %d L/m", (uint32_t)decode_float(msg_value_buffer));
  else if (msg_type == X_WLS_MSG_TYPE)
    set_x_wls(msg_value_buffer[0]);
  else if (msg_type == DEVICE_ERROR_MSG_TYPE)
    set_device_error(0xC0);

  if (cfg2 == cur_device_mode)
    return;
  cur_device_mode = cfg2;

  if (cfg2 == 0xA0 || cfg2 == 0x00)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, brass_slab);
  else if (cfg2 == 0xA1)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, stainless_steel_slab);
  else if (cfg2 == 0xA2)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, brass_cylinder_d20);
  else if (cfg2 == 0xA3)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, stainless_steel_cylinder_d20);
  else if (cfg2 == 0xA4)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, brass_cylinder_d30);
  else if (cfg2 == 0xA5)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, brass_sphere);
  else if (cfg2 == 0xA6)
    lv_label_set_text_fmt(lt_ht113_sample_label, "%s    LT-HT113 Sample: %s", LV_SYMBOL_STOP, stainless_steel_sphere);
  else
    set_device_error(0xC1);
}