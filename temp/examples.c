// void create_label() {
//   lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
//   lv_obj_t* label = lv_label_create(lv_screen_active());
//   lv_label_set_text(label, "LVGL Test");
//   lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
//   lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }

// =====================================================

// uint8_t count = 0;

// void btn_click(lv_event_t* event) {
//   printf("Click\n");
//   count++;
//   lv_obj_t* btn = lv_event_get_target(event);
//   lv_obj_t* btn_label = lv_obj_get_child(btn, 0);
//   lv_label_set_text_fmt(btn_label, "LVGL Click: %d", count);
// }

// void create_btn_state() {
//   lv_obj_t* btn = lv_button_create(lv_screen_active());
//   lv_obj_set_pos(btn, 10, 10);
//   lv_obj_set_size(btn, 150, 50);
//   lv_obj_t* btn_label = lv_label_create(btn);
//   lv_label_set_text_fmt(btn_label, "LVGL Click: %d", count);
//   lv_obj_center(btn_label);
//   lv_obj_add_event_cb(btn, btn_click, LV_EVENT_SHORT_CLICKED, NULL);
// }

// =====================================================

// void slider_event(lv_event_t* event) {
//   lv_obj_t* slider = lv_event_get_target(event);
//   uint8_t slider_value = lv_slider_get_value(slider);
//   lv_obj_t* slider_label = lv_obj_get_child(slider, 0);
//   lv_label_set_text_fmt(slider_label, "Slider: %d", 12);
// }

// void create_slider_state() {
//   lv_obj_t* slider = lv_slider_create(lv_screen_active());
//   lv_obj_set_width(slider, 400);
//   lv_obj_center(slider);
//   lv_obj_add_event_cb(slider, slider_event, LV_EVENT_VALUE_CHANGED, NULL);

//   lv_obj_t* slider_label = lv_label_create(lv_screen_active());
//   lv_label_set_text_fmt(slider_label, "Slider: %d", 0);
//   lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_TOP_MID, 0, -16);
// }
