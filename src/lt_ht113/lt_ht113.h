#include "lvgl/lvgl.h"

#include "../common/common.h"

// device binary images
#include "lt_ht113_brass_cylinder_d20.h"
#include "lt_ht113_brass_cylinder_d30.h"
#include "lt_ht113_brass_slab.h"
#include "lt_ht113_brass_sphere.h"
#include "lt_ht113_stainless_steel_cylinder_d20.h"
#include "lt_ht113_stainless_steel_slab.h"
#include "lt_ht113_stainless_steel_sphere.h"

#define BODY_CANVAS_WIDTH DEVICE_MODEL_IMG_WIDTH
#define BODY_CANVAS_HEIGHT DEVICE_MODEL_IMG_HEIGHT

// body canvas params
#define NUM_D_W 16
#define NUM_D_H 24
#define TXT_C_W 14
#define TXT_C_H 18
#define T_AMB_BC_POS 112, 67
#define T_SAM_BC_POS 201, 22
#define T_REF_BC_POS 458, 31
#define W_FLW_BC_POS 563, 354
#define X_WLS_BC_POS 567, 73

// emu spi params
#define T_SAM_MSG_TYPE 0
#define T_AMB_MSG_TYPE 1
#define T_REF_MSG_TYPE 2
#define W_FLW_MSG_TYPE 3
#define X_WLS_MSG_TYPE 4
#define DEVICE_ERROR_MSG_TYPE 14

#define CHX_DEBUG 1

static lv_color_t body_canvas_buffer[((32 / 8) * BODY_CANVAS_WIDTH * BODY_CANVAS_HEIGHT)];
lv_obj_t* body_canvas;
lv_draw_img_dsc_t img_gfx_info;
void lt_ht113_create_base_body_canvas() {
  body_canvas = lv_canvas_create(lv_scr_act());
  lv_canvas_set_buffer(body_canvas, body_canvas_buffer, BODY_CANVAS_WIDTH, BODY_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
  lv_canvas_fill_bg(body_canvas, CHX_DARK_BG_COLOR, LV_OPA_0);
  lv_obj_center(body_canvas);
#ifdef CHX_DEBUG
  lv_obj_set_style_outline_width(body_canvas, 2, LV_PART_MAIN);
  lv_obj_set_style_outline_color(body_canvas, CHX_ACCENT_COLOR, LV_PART_MAIN);
#endif
  lv_obj_add_flag(body_canvas, LV_OBJ_FLAG_CLICKABLE);
  lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_brass_slab, &img_gfx_info);
}

lv_draw_label_dsc_t label_temps_gfx_info;
lv_draw_rect_dsc_t label_digit_rect_gfx_info;
void init_gfx_info() {
  lv_draw_label_dsc_init(&label_temps_gfx_info);
  label_temps_gfx_info.color = CHX_FONT_COLOR;
  label_temps_gfx_info.font = &lv_font_montserrat_22;
  lv_draw_rect_dsc_init(&label_digit_rect_gfx_info);
  label_digit_rect_gfx_info.bg_color = CHX_DARK_BG_COLOR;
  lv_draw_img_dsc_init(&img_gfx_info);
}
void write_t_amb(float t_amb_val) {
  char val_str[3];
  sprintf(val_str, "%02d", (int)t_amb_val);
  lv_canvas_draw_rect(body_canvas, T_AMB_BC_POS, NUM_D_W * 2, NUM_D_H, &label_digit_rect_gfx_info);
  lv_canvas_draw_text(body_canvas, T_AMB_BC_POS, NUM_D_W * 2, &label_temps_gfx_info, val_str);
}
void write_t_sam(float t_sam_val) {
  char val_str[3];
  sprintf(val_str, "%02d", (int)t_sam_val);
  lv_canvas_draw_rect(body_canvas, T_SAM_BC_POS, NUM_D_W * 2, NUM_D_H, &label_digit_rect_gfx_info);
  lv_canvas_draw_text(body_canvas, T_SAM_BC_POS, NUM_D_W * 2, &label_temps_gfx_info, val_str);
}
void write_t_ref(float t_ref_val) {
  char val_str[3];
  sprintf(val_str, "%02d", (int)t_ref_val);
  lv_canvas_draw_rect(body_canvas, T_REF_BC_POS, NUM_D_W * 2, NUM_D_H, &label_digit_rect_gfx_info);
  lv_canvas_draw_text(body_canvas, T_REF_BC_POS, NUM_D_W * 2, &label_temps_gfx_info, val_str);
}
void write_w_flw(float w_flw_val) {
  char val_str[4];
  sprintf(val_str, "%03d", (int)w_flw_val);
  lv_canvas_draw_rect(body_canvas, W_FLW_BC_POS, NUM_D_W * 3 + 2, NUM_D_H, &label_digit_rect_gfx_info);
  lv_draw_label_dsc_t label_w_flw_gfx_info;
  lv_draw_label_dsc_init(&label_w_flw_gfx_info);
  label_w_flw_gfx_info.color = lv_color_hex(0x2196F3);
  label_w_flw_gfx_info.font = &lv_font_montserrat_22;
  lv_canvas_draw_text(body_canvas, W_FLW_BC_POS, NUM_D_W * 3, &label_w_flw_gfx_info, val_str);
}
void write_x_wls(uint8_t wls_state) {
  lv_draw_label_dsc_t label_x_wls_gfx_info;
  lv_draw_label_dsc_init(&label_x_wls_gfx_info);
  label_x_wls_gfx_info.font = &lv_font_montserrat_18;
  char* wls_label;
  char* wls_on_label = "ON";
  char* wls_off_label = "OFF";
  if (wls_state == 0) {
    wls_label = wls_off_label;
    label_x_wls_gfx_info.color = lv_color_hex(0xDD2C00);
  } else {
    wls_label = wls_on_label;
    label_x_wls_gfx_info.color = lv_color_hex(0x64DD17);
  }
  lv_canvas_draw_rect(body_canvas, X_WLS_BC_POS, TXT_C_W * 3, TXT_C_H, &label_digit_rect_gfx_info);
  lv_canvas_draw_text(body_canvas, X_WLS_BC_POS, TXT_C_W * 3, &label_x_wls_gfx_info, wls_label);
}
void init_labels() {
  write_t_amb(0);
  write_t_sam(0);
  write_t_ref(0);
  write_w_flw(0);
  write_x_wls(0);
}

#ifdef CHX_DEBUG
// mouse debug
lv_obj_t* mouse_debug_label;
void mouse_debug_event(lv_event_t* lv_event) {
  lv_event_code_t event_code = lv_event_get_code(lv_event);
  if (event_code == LV_EVENT_CLICKED) {
    lv_point_t mouse_point;
    lv_indev_get_point(lv_indev_get_act(), &mouse_point);
    lv_obj_t* event_target = lv_event_get_target(lv_event);
    mouse_point.x -= event_target->coords.x1;
    mouse_point.y -= event_target->coords.y1;
    char mouse_debug_label_text[32];
    sprintf(mouse_debug_label_text, "MOUSE | x: %d, y: %d", mouse_point.x, mouse_point.y);
    lv_label_set_text(mouse_debug_label, mouse_debug_label_text);
  }
}
void enable_mouse_debug() {
  mouse_debug_label = lv_label_create(lv_scr_act());
  lv_obj_set_x(mouse_debug_label, LT_HMI_HOR_RES / 2);
  lv_obj_set_y(mouse_debug_label, LT_HMI_VER_RES - 20);
  lv_obj_set_style_text_color(mouse_debug_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_add_event_cb(body_canvas, mouse_debug_event, LV_EVENT_ALL, NULL);
  lv_label_set_text(mouse_debug_label, "MOUSE | x: 0, y: 0");
}

// button debug
void debug_btn_click(lv_event_t* lv_event) {
  lv_event_code_t event_code = lv_event_get_code(lv_event);
  if (event_code == LV_EVENT_CLICKED) {
  }
}
void enable_debug_btn() {
  lv_obj_t* debug_button = lv_btn_create(lv_scr_act());
  lv_obj_set_x(debug_button, LT_HMI_HOR_RES / 2 + 400);
  lv_obj_set_y(debug_button, LT_HMI_VER_RES - 40);
  lv_obj_set_width(debug_button, 100);
  lv_obj_set_height(debug_button, 30);
  lv_obj_add_event_cb(debug_button, debug_btn_click, LV_EVENT_ALL, NULL);
}

// emu_spi debug
lv_obj_t* emu_spi_test_label;
uint8_t cur_device_mode = 0x00;
void enable_emu_spi_debug() {
  emu_spi_test_label = lv_label_create(lv_scr_act());
  lv_obj_set_x(emu_spi_test_label, LT_HMI_HOR_RES / 2 - 400);
  lv_obj_set_y(emu_spi_test_label, LT_HMI_VER_RES - 20);
  lv_obj_set_style_text_color(emu_spi_test_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_label_set_text(emu_spi_test_label, "EMU_SPI | msg_type: XX, msg_value: XX.XXX");
}
float decode_float(uint8_t* data_buffer) {
  float decoded_value = 0;
  ((uint8_t*)&decoded_value)[0] = data_buffer[0];
  ((uint8_t*)&decoded_value)[1] = data_buffer[1];
  ((uint8_t*)&decoded_value)[2] = data_buffer[2];
  ((uint8_t*)&decoded_value)[3] = data_buffer[3];
  return decoded_value;
}
void handle_emu_spi_device_msg(uint8_t msg_type, uint8_t cfg2, uint8_t* msg_value_buffer) {
  // handle LT_HT113 msg_type
  printf("EMU_SPI | Msg Type: %d, cfg2: %d\n", msg_type, cfg2);
  if (msg_type == T_SAM_MSG_TYPE)
    write_t_sam(decode_float(msg_value_buffer));
  else if (msg_type == T_AMB_MSG_TYPE)
    write_t_amb(decode_float(msg_value_buffer));
  else if (msg_type == T_REF_MSG_TYPE)
    write_t_ref(decode_float(msg_value_buffer));
  else if (msg_type == W_FLW_MSG_TYPE)
    write_w_flw(decode_float(msg_value_buffer));
  else if (msg_type == X_WLS_MSG_TYPE)
    write_x_wls(msg_value_buffer[0]);
  else
    printf("EMU_SPI | UNKNOWN Msg Type: %d\n", msg_type);

  if (cfg2 == cur_device_mode)
    return;

  // handle LT_HT113 cfg2
  cur_device_mode = cfg2;
  lv_canvas_fill_bg(body_canvas, CHX_DARK_BG_COLOR, LV_OPA_0);
  if (cfg2 == 0xA0 || cfg2 == 0x00)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_brass_slab, &img_gfx_info);
  else if (cfg2 == 0xA1)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_stainless_steel_slab, &img_gfx_info);
  else if (cfg2 == 0xA2)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_brass_cylinder_d20, &img_gfx_info);
  else if (cfg2 == 0xA3)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_stainless_steel_cylinder_d20, &img_gfx_info);
  else if (cfg2 == 0xA4)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_brass_cylinder_d30, &img_gfx_info);
  else if (cfg2 == 0xA5)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_brass_sphere, &img_gfx_info);
  else if (cfg2 == 0xA6)
    lv_canvas_draw_img(body_canvas, 0, 0, &lt_ht113_stainless_steel_sphere, &img_gfx_info);
  else
    printf("EMU_SPI | UNKNOWN cfg2: %d\n", cfg2);
  init_labels();
}
#endif

void lt_ht113_main_screen_create() {
  create_lt_frame("LT-HT113");
  init_gfx_info();
  lt_ht113_create_base_body_canvas();
  init_labels();

#ifdef CHX_DEBUG
  enable_mouse_debug();
  enable_debug_btn();
  enable_emu_spi_debug();
#endif
}