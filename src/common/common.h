#include "lvgl/lvgl.h"

#include "./lt_logo.h"

#define LT_HMI_HOR_RES 1280
#define LT_HMI_VER_RES 800

#define TOP_BAR_CANVAS_WIDTH LT_HMI_HOR_RES
#define TOP_BAR_CANVAS_HEIGHT 60

#define DEVICE_MODEL_IMG_WIDTH 800
#define DEVICE_MODEL_IMG_HEIGHT 500

#define CHX_DARK_BG_COLOR lv_color_hex(0x0B0E1F)
#define CHX_ACCENT_COLOR lv_color_hex(0x29B2F8)
#define CHX_FONT_COLOR lv_color_hex(0x8BA2CC)

static lv_color_t top_bar_canvas_buffer[((32 / 8) * TOP_BAR_CANVAS_WIDTH * TOP_BAR_CANVAS_HEIGHT)];

void create_lt_frame(const char* device_model) {
  lv_obj_set_style_bg_color(lv_scr_act(), CHX_DARK_BG_COLOR, LV_PART_MAIN);

  // top bar
  lv_obj_t* top_bar_canvas = lv_canvas_create(lv_scr_act());
  lv_obj_set_x(top_bar_canvas, 0);
  lv_obj_set_y(top_bar_canvas, 0);
  lv_canvas_set_buffer(top_bar_canvas, top_bar_canvas_buffer, TOP_BAR_CANVAS_WIDTH, TOP_BAR_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
  lv_canvas_fill_bg(top_bar_canvas, CHX_DARK_BG_COLOR, LV_OPA_0);
  lv_obj_set_style_outline_width(top_bar_canvas, 4, LV_PART_MAIN);
  lv_obj_set_style_outline_color(top_bar_canvas, CHX_ACCENT_COLOR, LV_PART_MAIN);
  lv_obj_set_style_outline_pad(top_bar_canvas, 0, LV_PART_MAIN);

  // device model tag
  lv_draw_rect_dsc_t device_tag_gfx_info;
  lv_draw_rect_dsc_init(&device_tag_gfx_info);
  device_tag_gfx_info.bg_color = CHX_ACCENT_COLOR;
  const lv_point_t device_tag_polygon_points[] = {
      {.x = 0, .y = 0},
      {.x = 250, .y = 0},
      {.x = 300, .y = TOP_BAR_CANVAS_HEIGHT},
      {.x = 0, .y = TOP_BAR_CANVAS_HEIGHT},
  };
  lv_canvas_draw_polygon(top_bar_canvas, device_tag_polygon_points, 4, &device_tag_gfx_info);
  lv_draw_label_dsc_t device_tag_text_gfx_info;
  lv_draw_label_dsc_init(&device_tag_text_gfx_info);
  device_tag_text_gfx_info.color = lv_color_hex(0xFFFFFF);
  device_tag_text_gfx_info.font = &lv_font_montserrat_16;
  lv_canvas_draw_text(top_bar_canvas, 110, TOP_BAR_CANVAS_HEIGHT / 2 - 8, 100, &device_tag_text_gfx_info, device_model);

  // draw lt logo
  lv_draw_img_dsc_t lt_logo_gfx_info;
  lv_draw_img_dsc_init(&lt_logo_gfx_info);
  lt_logo_gfx_info.recolor = CHX_FONT_COLOR;
  lv_canvas_draw_img(top_bar_canvas, TOP_BAR_CANVAS_WIDTH - 239, TOP_BAR_CANVAS_HEIGHT / 2 - 16, &lt_logo, &lt_logo_gfx_info);
}