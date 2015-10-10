#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer_number;
static int m1;
static int m2;
static char s_s1[32];
static char s_s2[32];
static int number;

static void vibes_veryshort_pulse() {
  static const uint32_t const segments[] = { 50 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);

}

static void refresh_number(int curr) {
  number += curr;
  snprintf(s_s1, sizeof(s_s1), "%d (%d)", number, curr);
  text_layer_set_text(text_layer_number, s_s1);
}





static void up_down_handler(ClickRecognizerRef recognizer, void *context) {
  refresh_number(5);
  text_layer_set_text(text_layer, "up down");
}
static void up_long_handler(ClickRecognizerRef recognizer, void *context) {
  refresh_number(5);
  vibes_veryshort_pulse();
  text_layer_set_text(text_layer, "up long");
  //app_timer_register(500, timer_callback, NULL);
}


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  number = 0;
  refresh_number(0);
  text_layer_set_text(text_layer, "Select");
}



static void down_down_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Szia Ajna!");
  refresh_number(1);
}





static void timer_callback(void *data) {
  vibes_veryshort_pulse();
  text_layer_set_text(text_layer, "timer");
}

static void select_long_down_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_veryshort_pulse();
  m1++;
  snprintf(s_s1, sizeof(s_s1), "long down %d", m1);
  text_layer_set_text(text_layer, s_s1);
  app_timer_register(500, timer_callback, NULL);
}
static void select_long_up_handler(ClickRecognizerRef recognizer, void *context) {
  m2++;
  snprintf(s_s1, sizeof(s_s1), "long up %d", m2);
  text_layer_set_text(text_layer, s_s1);
}
static void select_down_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "S down");
}
static void select_up_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "S up");
}







static void click_config_provider(void *context) {
  window_raw_click_subscribe(BUTTON_ID_UP, up_down_handler, NULL, NULL);
  window_long_click_subscribe(BUTTON_ID_UP, 300, up_long_handler, NULL);

  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);

  window_raw_click_subscribe(BUTTON_ID_DOWN, down_down_handler, NULL, NULL);

  //window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 300, select_long_down_handler, select_long_up_handler);
  window_raw_click_subscribe(BUTTON_ID_SELECT, select_down_handler, select_up_handler, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer_number = text_layer_create((GRect) { .origin = { 2, 94 }, .size = { bounds.size.w - 4, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(text_layer_number));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  number = 0;

  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
