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

#define CFG2_BRSS_SLAB 0xA0
#define CFG2_STLS_SLAB 0xA1
#define CFG2_BRSS_CD20 0xA2
#define CFG2_STLS_CD20 0xA3
#define CFG2_BRSS_CD30 0xA4
#define CFG2_BRSS_SPHR 0xA5
#define CFG2_STLS_SPHR 0xA6

const static char* t_sam_label_str = "Sample Temprature:";
const static char* t_amb_label_str = "Ambient Temprature:";
const static char* t_ref_label_str = "Reference Temprature:";

const static char* brass_cylinder_d20 = "Brass Cylinder D=20mm";
const static char* brass_cylinder_d30 = "Brass Cylinder D=30mm";
const static char* brass_slab = "Brass Slab";
const static char* brass_sphere = "Brass Sphere";
const static char* stainless_steel_cylinder_d20 = "Stainless Steel Cylinder D=20mm";
const static char* stainless_steel_slab = "Stainless Steel Slab";
const static char* stainless_steel_sphere = "Stainless Steel Sphere";

const static char* LT_HT113_ERR_TCS_OPT = "Temperature {T_sam} Over Temperature Protection";
const static char* LT_HT113_ERR_TCA_OPT = "Temperature {T_amb} Over Temperature Protection";
const static char* LT_HT113_ERR_TCR_OPT = "Temperature {T_ref} Over Temperature Protection";
const static char* LT_HT113_ERR_PMP_DRE = "Pump Drive Error";

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
  lv_obj_align(device_img_cont, LV_ALIGN_TOP_LEFT, 0, 0);
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
  lv_obj_align(readings_panel, LV_ALIGN_TOP_LEFT, DEVICE_IMG_WIDTH, 0);
  lv_obj_set_style_bg_color(readings_panel, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_border_width(readings_panel, 0, LV_PART_MAIN);
}

lv_obj_t* lt_ht113_sample_cont;
lv_obj_t* lt_ht113_sample_label;
lv_obj_t* lt_ht113_sample_shape;
void set_lt_ht113_sample(uint8_t cfg2) {
  if (cfg2 == CFG2_BRSS_CD20) {
    lv_obj_set_size(lt_ht113_sample_shape, 80, 20);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 4, LV_PART_MAIN);
  } else if (cfg2 == CFG2_BRSS_CD30) {
    lv_obj_set_size(lt_ht113_sample_shape, 80, 30);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 4, LV_PART_MAIN);
  } else if (cfg2 == CFG2_BRSS_SLAB) {
    lv_obj_set_size(lt_ht113_sample_shape, 40, 40);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 0, LV_PART_MAIN);
  } else if (cfg2 == CFG2_BRSS_SPHR) {
    lv_obj_set_size(lt_ht113_sample_shape, 40, 40);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 20, LV_PART_MAIN);
  } else if (cfg2 == CFG2_STLS_CD20) {
    lv_obj_set_size(lt_ht113_sample_shape, 80, 20);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xD5D5D7), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 4, LV_PART_MAIN);
  } else if (cfg2 == CFG2_STLS_SLAB) {
    lv_obj_set_size(lt_ht113_sample_shape, 40, 40);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xD5D5D7), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 0, LV_PART_MAIN);
  } else if (cfg2 == CFG2_STLS_SPHR) {
    lv_obj_set_size(lt_ht113_sample_shape, 40, 40);
    lv_obj_align(lt_ht113_sample_shape, LV_ALIGN_RIGHT_MID, -100, 0);
    lv_obj_set_style_bg_color(lt_ht113_sample_shape, lv_color_hex(0xD5D5D7), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht113_sample_shape, 20, LV_PART_MAIN);
  }
}

lv_obj_t* t_sam_cont;
lv_obj_t* t_sam_label;
void create_device_reading_t_sam() {
  t_sam_cont = lv_obj_create(readings_panel);
  t_sam_label = lv_label_create(t_sam_cont);
  style_neon_div(lv_color_hex(0x9C27B0), t_sam_cont, t_sam_label, READINGS_PANEL_SIZE - 48, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(t_sam_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_align(t_sam_cont, LV_ALIGN_LEFT_MID, 0, 0);
  lv_label_set_text_fmt(t_sam_label, "%s    Sample Temprature: NAN Celsius", LV_SYMBOL_RIGHT);
}

lv_obj_t* t_amb_cont;
lv_obj_t* t_amb_label;
void create_device_reading_t_amb() {
  t_amb_cont = lv_obj_create(readings_panel);
  t_amb_label = lv_label_create(t_amb_cont);
  style_neon_div(lv_color_hex(0x00BCD4), t_amb_cont, t_amb_label, READINGS_PANEL_SIZE - 48, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(t_amb_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_align_to(t_amb_cont, t_sam_cont, LV_ALIGN_OUT_TOP_LEFT, 0, -READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(t_amb_label, "%s    Ambient Temprature: NAN Celsius", LV_SYMBOL_RIGHT);
}

lv_obj_t* t_ref_cont;
lv_obj_t* t_ref_label;
void create_device_reading_t_ref() {
  t_ref_cont = lv_obj_create(readings_panel);
  t_ref_label = lv_label_create(t_ref_cont);
  style_neon_div(lv_color_hex(0x009688), t_ref_cont, t_ref_label, READINGS_PANEL_SIZE - 48, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(t_ref_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_align_to(t_ref_cont, t_sam_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(t_ref_label, "%s    Reference Temprature: NAN Celsius", LV_SYMBOL_RIGHT);
}

lv_obj_t* w_flw_cont;
lv_obj_t* w_flw_label;
void create_device_reading_w_flw() {
  w_flw_cont = lv_obj_create(readings_panel);
  w_flw_label = lv_label_create(w_flw_cont);
  style_neon_div(lv_color_hex(0x2196F3), w_flw_cont, w_flw_label, 300, LV_SIZE_CONTENT);
  lv_obj_center(w_flw_label);
  lv_obj_set_style_bg_opa(w_flw_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_set_style_text_font(w_flw_label, &lv_font_montserrat_16, LV_PART_MAIN);
  lv_obj_align_to(w_flw_cont, t_ref_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(w_flw_label, "Water Flow: NAN L/m");
}

lv_obj_t* wls_warn_cont;
lv_obj_t* wls_warn_label;
void create_wls_warning() {
  wls_warn_cont = lv_obj_create(readings_panel);
  wls_warn_label = lv_label_create(wls_warn_cont);
  style_neon_div(CHX_SUCCESS_COLOR, wls_warn_cont, wls_warn_label, READINGS_PANEL_SIZE - 48, LV_SIZE_CONTENT);
  // lv_obj_align(wls_warn_cont, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_align_to(wls_warn_cont, w_flw_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, READINGS_PANEL_MARGIN);
  lv_label_set_text_fmt(wls_warn_label, "%s    %s", LV_SYMBOL_OK, "WATER LEVEL - OK");
}

void create_lt_ht113_sample() {
  lt_ht113_sample_cont = lv_obj_create(readings_panel);
  lt_ht113_sample_label = lv_label_create(lt_ht113_sample_cont);
  style_neon_div(CHX_ACCENT_COLOR, lt_ht113_sample_cont, lt_ht113_sample_label, READINGS_PANEL_SIZE - 48, 100);
  // lv_obj_align(lt_ht113_sample_cont, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_obj_align_to(lt_ht113_sample_cont, t_amb_cont, LV_ALIGN_OUT_TOP_MID, 0, 0);
  lv_obj_set_style_bg_opa(lt_ht113_sample_cont, LV_OPA_0, LV_PART_MAIN);
  lv_obj_set_style_border_width(lt_ht113_sample_cont, 0, LV_PART_MAIN);
  lv_label_set_text(lt_ht113_sample_label, "LT-HT113 Sample: --");

  lt_ht113_sample_shape = lv_obj_create(lt_ht113_sample_cont);
  lv_obj_set_style_pad_all(lt_ht113_sample_shape, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(lt_ht113_sample_shape, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(lt_ht113_sample_shape, 0, LV_PART_MAIN);
}

void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer);

void init_hmi_state() {
  float _x = LT_HMI_NAN;
  uint8_t init_dmsg_value_buffer[8] = {0};
  init_dmsg_value_buffer[0] = ((uint8_t*)&_x)[0];
  init_dmsg_value_buffer[1] = ((uint8_t*)&_x)[1];
  init_dmsg_value_buffer[2] = ((uint8_t*)&_x)[2];
  init_dmsg_value_buffer[3] = ((uint8_t*)&_x)[3];
  handle_device_msg(0, CFG2_BRSS_SLAB, init_dmsg_value_buffer);
}

uint8_t hmi_ready = 0;
void lt_ht113_main_screen_create() {
  create_lt_frame("LT-HT113");
  create_inner_window();
  create_device_img();
  create_readings_panel();
  create_device_reading_t_sam();
  create_device_reading_t_amb();
  create_device_reading_t_ref();
  create_device_reading_w_flw();
  create_lt_ht113_sample();
  create_wls_warning();
  create_error_toasts(600);
  set_device_title("Unsteady Heat Transfer Apparatus");
  hmi_ready = 1;
  init_hmi_state();
}

void set_x_wls(uint8_t wls_state) {
  if (wls_state == 0) {
    lv_obj_set_style_bg_color(wls_warn_cont, CHX_WARNING_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_color(wls_warn_cont, CHX_WARNING_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_color(wls_warn_label, CHX_WARNING_COLOR, LV_PART_MAIN);
    lv_label_set_text_fmt(wls_warn_label, "%s    %s", LV_SYMBOL_WARNING, "WATER LEVEL - INSUFFICIENT");
  } else {
    lv_obj_set_style_bg_color(wls_warn_cont, CHX_SUCCESS_COLOR, LV_PART_MAIN);
    lv_obj_set_style_border_color(wls_warn_cont, CHX_SUCCESS_COLOR, LV_PART_MAIN);
    lv_obj_set_style_text_color(wls_warn_label, CHX_SUCCESS_COLOR, LV_PART_MAIN);
    lv_label_set_text_fmt(wls_warn_label, "%s    %s", LV_SYMBOL_OK, "WATER LEVEL - OK");
  }
}

void add_device_error(uint8_t device_error) {
  if (device_error == 0xF0)
    add_error_toast(LT_HT113_ERR_TCS_OPT);
  else if (device_error == 0xF1)
    add_error_toast(LT_HT113_ERR_TCA_OPT);
  else if (device_error == 0xF2)
    add_error_toast(LT_HT113_ERR_TCR_OPT);
  else if (device_error == 0xF3)
    add_error_toast(LT_HT113_ERR_PMP_DRE);
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
void set_temp_value(lv_obj_t* temp_label, const char* temp_label_str, float temp_value) {
  if ((int)temp_value == LT_HMI_NAN) {
    lv_label_set_text_fmt(temp_label, "%s    %s NAN Celsius", LV_SYMBOL_RIGHT, temp_label_str);
    return;
  }
  lv_label_set_text_fmt(temp_label, "%s    %s %.1f Celsius", LV_SYMBOL_RIGHT, temp_label_str, temp_value);
}
void set_w_flw(lv_obj_t* w_flw_label, float w_flw_value) {
  if ((int)w_flw_value == -LT_HMI_NAN) {
    lv_label_set_text_fmt(w_flw_label, "Water Flow: NAN L/m");
    return;
  }
  lv_label_set_text_fmt(w_flw_label, "Water Flow: %.1f L/m", w_flw_value);
}
void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer) {
  if (!hmi_ready)
    return;

  // handle LT_HT113 msg_type
  if (msg_type == T_SAM_MSG_TYPE)
    set_temp_value(t_sam_label, t_sam_label_str, decode_float(msg_value_buffer));
  else if (msg_type == T_AMB_MSG_TYPE)
    set_temp_value(t_amb_label, t_amb_label_str, decode_float(msg_value_buffer));
  else if (msg_type == T_REF_MSG_TYPE)
    set_temp_value(t_ref_label, t_ref_label_str, decode_float(msg_value_buffer));
  else if (msg_type == W_FLW_MSG_TYPE)
    lv_label_set_text_fmt(w_flw_label, "Water Flow: %.1f L/m", decode_float(msg_value_buffer));
  else if (msg_type == X_WLS_MSG_TYPE)
    set_x_wls(msg_value_buffer[0]);
  else if (msg_type == DEVICE_ERROR_MSG_TYPE)
    add_device_error(msg_value_buffer[0]);
  else
    add_device_error(0xC0);

  if (cfg2 == cur_device_mode)
    return;
  cur_device_mode = cfg2;

  if (cfg2 == CFG2_BRSS_SLAB || cfg2 == 0x00)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", brass_slab);
  else if (cfg2 == CFG2_STLS_SLAB)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", stainless_steel_slab);
  else if (cfg2 == CFG2_BRSS_CD20)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", brass_cylinder_d20);
  else if (cfg2 == CFG2_STLS_CD20)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", stainless_steel_cylinder_d20);
  else if (cfg2 == CFG2_BRSS_CD30)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", brass_cylinder_d30);
  else if (cfg2 == CFG2_BRSS_SPHR)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", brass_sphere);
  else if (cfg2 == CFG2_STLS_SPHR)
    lv_label_set_text_fmt(lt_ht113_sample_label, "LT-HT113 Sample: %s", stainless_steel_sphere);
  else
    add_device_error(0xC1);
  set_lt_ht113_sample(cfg2);
}
