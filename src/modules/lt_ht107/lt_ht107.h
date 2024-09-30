#include "lvgl/lvgl.h"

#include "../../lib/common.h"
#include "./lt_ht107_linear.h"
#include "./lt_ht107_radial.h"

#define DEVICE_ERROR_MSG_TYPE 14

#define CFG2_SAMPLE_DISCONNECTED 0xA0
#define CFG2_LINEAR_1 0xA1
#define CFG2_LINEAR_2 0xA2
#define CFG2_LINEAR_3 0xA3
#define CFG2_RADIAL 0xA4

#define CHART_POINT_TOMBSTONE 0x7FFF

const char* SAMPLE_DISCONNECTED_TEXT = "No Sample is Connected";
const char* LINEAR_1_SAMPLE_TEXT = "Linear Brass: D=25mm, L=30mm";
const char* LINEAR_2_SAMPLE_TEXT = "Linear Stainless Steel: D=25mm, L=30mm";
const char* LINEAR_3_SAMPLE_TEXT = "Linear Brass: D=15, L=30mm";
const char* RADIAL_X_SAMPLE_TEXT = "Radial Brass: D=110m, Thick=8mm";

const char* LT_HT113_ERR_TX_OTP[] = {
    "Temperature {T1} Over Temperature Protection",
    "Temperature {T2} Over Temperature Protection",
    "Temperature {T3} Over Temperature Protection",
    "Temperature {T4} Over Temperature Protection",
    "Temperature {T5} Over Temperature Protection",
    "Temperature {T6} Over Temperature Protection",
    "Temperature {T7} Over Temperature Protection",
    "Temperature {T8} Over Temperature Protection",
    "Temperature {T9} Over Temperature Protection",
};

lv_obj_t* device_img_linear;
lv_obj_t* device_img_radial;
#define ACTIVE_BORDER_W 2
void create_images_section() {
  device_img_radial = lv_img_create(lv_scr_act());
  lv_obj_set_size(device_img_radial, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align_to(device_img_radial, device_title, LV_ALIGN_BOTTOM_LEFT, -320, 32);
  lv_obj_set_style_border_width(device_img_radial, ACTIVE_BORDER_W, LV_PART_MAIN);
  lv_obj_set_style_border_color(device_img_radial, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_radius(device_img_radial, 4, LV_PART_MAIN);
  decompress_lt_ht107_radial();
  lv_img_set_src(device_img_radial, &lt_ht107_radial);
  device_img_linear = lv_img_create(lv_scr_act());
  lv_obj_set_size(device_img_linear, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_align_to(device_img_linear, device_img_radial, LV_ALIGN_BOTTOM_LEFT, 0, 32);
  // lv_obj_set_style_border_width(device_img_linear, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(device_img_linear, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_radius(device_img_linear, 4, LV_PART_MAIN);
  decompress_lt_ht107_linear();
  lv_img_set_src(device_img_linear, &lt_ht107_linear);
}

lv_obj_t* position_heat_chart;
lv_chart_series_t* position_heat_chart_series;
void adapt_position_heat_chart_axes(uint8_t points_count, uint8_t y_min, uint8_t y_max) {
  uint8_t y_axis_lines_count = (y_max - y_min) / 10 + 1;
  lv_chart_set_point_count(position_heat_chart, points_count);
  lv_chart_set_div_line_count(position_heat_chart, y_axis_lines_count, points_count);
  lv_chart_set_range(position_heat_chart, LV_CHART_AXIS_PRIMARY_X, 0, points_count - 1);
  lv_chart_set_range(position_heat_chart, LV_CHART_AXIS_PRIMARY_Y, y_min, y_max);
  lv_chart_set_axis_tick(position_heat_chart, LV_CHART_AXIS_PRIMARY_X, 8, 4, points_count, 5, true, 32);
  lv_chart_set_axis_tick(position_heat_chart, LV_CHART_AXIS_PRIMARY_Y, 8, 4, y_axis_lines_count, 5, true, 32);
  for (uint8_t i = 0; i < points_count; i++)
    position_heat_chart_series->y_points[i] = 30;
  lv_chart_refresh(position_heat_chart);
}
void create_position_heat_chart() {
  position_heat_chart = lv_chart_create(lv_scr_act());
  lv_obj_set_size(position_heat_chart, 700, 350);
  lv_obj_align_to(position_heat_chart, device_img_radial, LV_ALIGN_OUT_RIGHT_TOP, 50, 0);
  lv_obj_set_style_border_width(position_heat_chart, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(position_heat_chart, CHX_EMPTY_GAUGE_COLOR, LV_PART_MAIN);
  lv_obj_set_style_radius(position_heat_chart, 4, LV_PART_MAIN);
  lv_obj_set_style_bg_color(position_heat_chart, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_line_color(position_heat_chart, CHX_EMPTY_GAUGE_COLOR, LV_PART_MAIN);
  lv_obj_set_style_line_color(position_heat_chart, CHX_EMPTY_GAUGE_COLOR, LV_PART_TICKS);
  position_heat_chart_series = lv_chart_add_series(position_heat_chart, CHX_FONT_COLOR, LV_CHART_AXIS_PRIMARY_Y);
  adapt_position_heat_chart_axes(9, 20, 100);
}

lv_obj_t* lt_ht107_sample_label;
lv_obj_t* lt_ht107_sample_shape;
void create_lt_ht107_sample() {
  lt_ht107_sample_label = lv_label_create(lv_scr_act());
  lv_obj_set_size(lt_ht107_sample_label, 400, LV_SIZE_CONTENT);
  lv_obj_align_to(lt_ht107_sample_label, position_heat_chart, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 44);
  lv_obj_set_style_text_color(lt_ht107_sample_label, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_text_font(lt_ht107_sample_label, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_label_set_text(lt_ht107_sample_label, RADIAL_X_SAMPLE_TEXT);

  lt_ht107_sample_shape = lv_obj_create(lv_scr_act());
  lv_obj_set_size(lt_ht107_sample_shape, 40, 40);
  lv_obj_align_to(lt_ht107_sample_shape, lt_ht107_sample_label, LV_ALIGN_OUT_RIGHT_MID, 100, 0);
  lv_obj_set_style_pad_all(lt_ht107_sample_shape, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(lt_ht107_sample_shape, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(lt_ht107_sample_shape, 0, LV_PART_MAIN);
}

lv_obj_t* heater_power_label;
lv_obj_t* heater_power_label_cont;
void create_heater_power_label() {
  heater_power_label_cont = lv_obj_create(lv_scr_act());
  lv_obj_set_size(heater_power_label_cont, 700, LV_SIZE_CONTENT);
  lv_obj_align_to(heater_power_label_cont, lt_ht107_sample_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 24);
  lv_obj_set_style_radius(heater_power_label_cont, 4, LV_PART_MAIN);
  lv_obj_set_style_border_width(heater_power_label_cont, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(heater_power_label_cont, CHX_FONT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_bg_color(heater_power_label_cont, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_pad_left(heater_power_label_cont, 24, LV_PART_MAIN);
  lv_obj_set_style_pad_ver(heater_power_label_cont, 12, LV_PART_MAIN);

  heater_power_label = lv_label_create(heater_power_label_cont);
  lv_obj_set_style_text_color(heater_power_label, CHX_FONT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_text_font(heater_power_label, &lv_font_montserrat_18, LV_PART_MAIN);
  lv_label_set_text_fmt(heater_power_label, "%s    Heater Power (P_Heater): NAN W", LV_SYMBOL_CHARGE);
}

lv_obj_t* temp_cont;
lv_obj_t* temp_labels[9];
const uint32_t temp_labels_colors[9] = {
    0x4CAF50,
    0x9E9E9E,
    0x3F51B5,
    0x2196F3,
    0xFF9800,
    0x9C27B0,
    0x009688,
    0xCDDC39,
    0x795548,
};
#define LT_HT107_TEMP_LABEL_SPACE 100
void create_temp_sequence() {
  temp_cont = lv_obj_create(lv_scr_act());
  lv_obj_set_size(temp_cont, 700, LV_SIZE_CONTENT);
  lv_obj_align_to(temp_cont, heater_power_label_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 16);
  lv_obj_set_style_radius(temp_cont, 4, LV_PART_MAIN);
  lv_obj_set_style_border_width(temp_cont, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(temp_cont, lv_color_hex(0xFFAB00), LV_PART_MAIN);
  lv_obj_set_style_bg_color(temp_cont, CHX_DARK_BG_COLOR, LV_PART_MAIN);
  lv_obj_set_style_pad_left(temp_cont, 24, LV_PART_MAIN);
  lv_obj_set_style_pad_ver(temp_cont, 12, LV_PART_MAIN);

  for (uint8_t i = 0; i < sizeof(temp_labels) / sizeof(lv_obj_t*); i++) {
    temp_labels[i] = lv_label_create(temp_cont);
    lv_obj_set_style_text_font(temp_labels[i], &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(temp_labels[i], lv_color_hex(temp_labels_colors[i]), LV_PART_MAIN);
    lv_label_set_text_fmt(temp_labels[i], "T%d: NAN", i + 1);
  }

  // align temp row 1
  lv_obj_align(temp_labels[1], LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_align_to(temp_labels[0], temp_labels[1], LV_ALIGN_OUT_LEFT_TOP, -LT_HT107_TEMP_LABEL_SPACE, 0);
  lv_obj_align_to(temp_labels[2], temp_labels[1], LV_ALIGN_OUT_RIGHT_TOP, LT_HT107_TEMP_LABEL_SPACE, 0);

  // align temp row 2
  lv_obj_align_to(temp_labels[4], temp_labels[1], LV_ALIGN_OUT_BOTTOM_LEFT, 0, 32);
  lv_obj_align_to(temp_labels[3], temp_labels[4], LV_ALIGN_OUT_LEFT_TOP, -LT_HT107_TEMP_LABEL_SPACE, 0);
  lv_obj_align_to(temp_labels[5], temp_labels[4], LV_ALIGN_OUT_RIGHT_TOP, LT_HT107_TEMP_LABEL_SPACE, 0);

  // align temp row 3
  lv_obj_align_to(temp_labels[7], temp_labels[4], LV_ALIGN_OUT_BOTTOM_LEFT, 0, 32);
  lv_obj_align_to(temp_labels[6], temp_labels[7], LV_ALIGN_OUT_LEFT_TOP, -LT_HT107_TEMP_LABEL_SPACE, 0);
  lv_obj_align_to(temp_labels[8], temp_labels[7], LV_ALIGN_OUT_RIGHT_TOP, LT_HT107_TEMP_LABEL_SPACE, 0);
}

void toggle_temp_vars(uint8_t temp_vars_count) {
  if (temp_vars_count == 0) {
    for (uint8_t i = 0; i < sizeof(temp_labels) / sizeof(lv_obj_t*); i++)
      lv_label_set_text_fmt(temp_labels[i], "T%d: NAN", i + 1);
    lv_obj_set_style_text_opa(temp_labels[6], LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_text_opa(temp_labels[7], LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_text_opa(temp_labels[8], LV_OPA_100, LV_PART_MAIN);
    adapt_position_heat_chart_axes(0, 20, 100);
  } else if (temp_vars_count == 6) {
    lv_obj_set_style_text_opa(temp_labels[6], LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_text_opa(temp_labels[7], LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_text_opa(temp_labels[8], LV_OPA_0, LV_PART_MAIN);
    adapt_position_heat_chart_axes(6, 20, 100);
  } else if (temp_vars_count == 9) {
    lv_obj_set_style_text_opa(temp_labels[6], LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_text_opa(temp_labels[7], LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_text_opa(temp_labels[8], LV_OPA_100, LV_PART_MAIN);
    adapt_position_heat_chart_axes(9, 20, 100);
  }
}

void set_lt_ht107_mode(uint8_t cfg2) {
  if (cfg2 == CFG2_SAMPLE_DISCONNECTED) {
    lv_obj_set_size(lt_ht107_sample_shape, 0, 0);
    lv_label_set_text(lt_ht107_sample_label, SAMPLE_DISCONNECTED_TEXT);
    lv_obj_set_style_border_width(device_img_linear, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_radial, 0, LV_PART_MAIN);
    toggle_temp_vars(0);
  } else if (cfg2 == CFG2_LINEAR_1) {
    lv_obj_set_size(lt_ht107_sample_shape, 80, 30);
    lv_obj_align_to(lt_ht107_sample_shape, lt_ht107_sample_label, LV_ALIGN_OUT_RIGHT_MID, 100, 0);
    lv_obj_set_style_bg_color(lt_ht107_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht107_sample_shape, 4, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_linear, ACTIVE_BORDER_W, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_radial, 0, LV_PART_MAIN);
    lv_label_set_text(lt_ht107_sample_label, LINEAR_1_SAMPLE_TEXT);
    toggle_temp_vars(9);
  } else if (cfg2 == CFG2_LINEAR_2) {
    lv_obj_set_size(lt_ht107_sample_shape, 80, 30);
    lv_obj_align_to(lt_ht107_sample_shape, lt_ht107_sample_label, LV_ALIGN_OUT_RIGHT_MID, 100, 0);
    lv_obj_set_style_bg_color(lt_ht107_sample_shape, lv_color_hex(0xD5D5D7), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht107_sample_shape, 4, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_linear, ACTIVE_BORDER_W, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_radial, 0, LV_PART_MAIN);
    lv_label_set_text(lt_ht107_sample_label, LINEAR_2_SAMPLE_TEXT);
    toggle_temp_vars(9);
  } else if (cfg2 == CFG2_LINEAR_3) {
    lv_obj_set_size(lt_ht107_sample_shape, 80, 20);
    lv_obj_align_to(lt_ht107_sample_shape, lt_ht107_sample_label, LV_ALIGN_OUT_RIGHT_MID, 100, 0);
    lv_obj_set_style_bg_color(lt_ht107_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht107_sample_shape, 4, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_linear, ACTIVE_BORDER_W, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_radial, 0, LV_PART_MAIN);
    lv_label_set_text(lt_ht107_sample_label, LINEAR_3_SAMPLE_TEXT);
    toggle_temp_vars(9);
  } else if (cfg2 == CFG2_RADIAL) {
    lv_obj_set_size(lt_ht107_sample_shape, 40, 40);
    lv_obj_align_to(lt_ht107_sample_shape, lt_ht107_sample_label, LV_ALIGN_OUT_RIGHT_MID, 100, 0);
    lv_obj_set_style_bg_color(lt_ht107_sample_shape, lv_color_hex(0xBA9D65), LV_PART_MAIN);
    lv_obj_set_style_radius(lt_ht107_sample_shape, 20, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_linear, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(device_img_radial, ACTIVE_BORDER_W, LV_PART_MAIN);
    lv_label_set_text(lt_ht107_sample_label, RADIAL_X_SAMPLE_TEXT);
    toggle_temp_vars(6);
  }
}

lv_color_t _heater_color;
void set_heater_power_color(float _heater_power) {
  if (_heater_power > 0 && _heater_power <= 40)
    _heater_color = lv_color_hex(0x64DD17);
  else if (_heater_power > 45 && _heater_power <= 80)
    _heater_color = lv_color_hex(0xFFAB00);
  else if (_heater_power > 85 && _heater_power <= 120)
    _heater_color = CHX_ERROR_COLOR;

  lv_obj_set_style_border_color(heater_power_label_cont, _heater_color, LV_PART_MAIN);
  lv_obj_set_style_text_color(heater_power_label, _heater_color, LV_PART_MAIN);
  lv_chart_set_series_color(position_heat_chart, position_heat_chart_series, _heater_color);
  lv_chart_refresh(position_heat_chart);
}

uint8_t hmi_ready = 0;
uint8_t cur_device_mode = CFG2_SAMPLE_DISCONNECTED;
void lt_ht107_main_screen_create() {
  _heater_color = CHX_FONT_COLOR;
  create_lt_frame("LT-HT107");
  set_device_title("Linear and Radial Heat Conduction", 16);
  create_images_section();
  create_position_heat_chart();
  create_lt_ht107_sample();
  create_heater_power_label();
  create_temp_sequence();
  create_error_toasts(600);
  hmi_ready = 1;
  set_lt_ht107_mode(cur_device_mode);
}

void add_device_error(uint8_t device_error) {
  if (device_error >= 0xF1 && device_error <= 0xF9) {
    uint8_t err_idx = (device_error & 0x0F) - 1;
    add_error_toast(LT_HT113_ERR_TX_OTP[err_idx]);
    return;
  }

  if (device_error == 0xFA)
    add_error_toast("Temperature {T_Heater} Over Temperature Protection");
  else if (device_error == 0xFB)
    add_error_toast("Constant Power {P_Heater} Over Power Protection");
}

void handle_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer) {
  if (!hmi_ready)
    return;

  // handle LT_HT07 temp msg_type
  if (msg_type >= 0 && msg_type <= 8) {
    float temp_value = decode_float(msg_value_buffer);
    if ((int)temp_value == LT_HMI_NAN) {
      lv_label_set_text_fmt(temp_labels[msg_type], "T%d: NAN", msg_type + 1);
    } else {
      lv_label_set_text_fmt(temp_labels[msg_type], "T%d: %.1f", msg_type + 1, temp_value);
      position_heat_chart_series->y_points[msg_type] = (int)temp_value;
      lv_chart_refresh(position_heat_chart);
    }
  }

  // handle LT_HT107 p_heater msg_type
  if (msg_type == 13) {
    float p_heater_value = decode_float(msg_value_buffer);
    set_heater_power_color(p_heater_value);
    if ((int)p_heater_value == LT_HMI_NAN)
      lv_label_set_text_fmt(heater_power_label, "%s    Heater Power (P_Heater): NAN W", LV_SYMBOL_CHARGE);
    else
      lv_label_set_text_fmt(heater_power_label, "%s    Heater Power (P_Heater): %.1f W", LV_SYMBOL_CHARGE, p_heater_value);
  }

  // handle LT_HT107
  if (msg_type == DEVICE_ERROR_MSG_TYPE)
    add_device_error(msg_value_buffer[0]);

  if (cfg2 == cur_device_mode)
    return;
  cur_device_mode = cfg2;
  set_lt_ht107_mode(cfg2);
}
