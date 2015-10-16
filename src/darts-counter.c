#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer_number;

static TextLayer *text_points_sum;
static TextLayer *text_points_togo;

static int m1;
static int m2;
static char s_s1[32];
static char s_points_sum[11];
static char s_points_togo[11];
static char s_game_round[11];

static int game_sum;
static int game_round;
static int game_start;

static int curr_number;
static int curr_modifier;

static AppTimer *up_button_timer;
static AppTimer *down_button_timer;
static AppTimer *select_button_timer;

static int select_state;

static void vibes_veryshort_pulse() {
  static const uint32_t const segments[] = { 40 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);
}
static void vibes_veryshort_number(int count) {
  static const uint32_t const segments[] = { 40, 50 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  for(int i = 0; i < count; i++) {
    vibes_enqueue_custom_pattern(pat);
  }
}

static void refresh_number(int curr) {
  curr_number += curr;
  if (curr_number > 20) {
    curr_number = 25;
    text_layer_set_text(text_layer, "BULL");
  }
  snprintf(s_s1, sizeof(s_s1), "%d (%d)", curr_number, curr);
  text_layer_set_text(text_layer_number, s_s1);
}

void set_text_layer_d(TextLayer *layer, char *s, char *format, int d) {
  snprintf(s, 9, format, d);
  text_layer_set_text(layer, s);
}

static void set_text(char *s, char *format, int d) {
  snprintf(s, 10, format, d);
}



// UP BUTTON
static void up_button_timer_callback(void *data) {
  refresh_number(5);
  vibes_veryshort_pulse();
  up_button_timer = app_timer_register(500, up_button_timer_callback, NULL);
}
static void up_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  refresh_number(5);
  vibes_veryshort_pulse();
  up_button_timer = app_timer_register(500, up_button_timer_callback, NULL);
}
static void up_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_cancel(up_button_timer);
  text_layer_set_text(text_layer, "Up: +5");
}

// DOWN BUTTON
static void down_button_timer_callback(void *data) {
  refresh_number(1);
  vibes_veryshort_pulse();
  down_button_timer = app_timer_register(500, down_button_timer_callback, NULL);
}
static void down_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  refresh_number(1);
  vibes_veryshort_pulse();
  down_button_timer = app_timer_register(500, down_button_timer_callback, NULL);
}
static void down_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_cancel(down_button_timer);
  text_layer_set_text(text_layer, "Down: +1");
}


// SELECT BUTTON
static void select_button_timer_callback(void *data) {
  select_state++;
  vibes_veryshort_number(select_state + 1);
  if (select_state < 3) {
    select_button_timer = app_timer_register(500, select_button_timer_callback, NULL);
  }
  switch(select_state) {
    case 1: text_layer_set_text(text_layer, "DOUBLE"); break;
    case 2: text_layer_set_text(text_layer, "TRIPLE"); break;
    case 3: text_layer_set_text(text_layer, "CANCEL"); break;
  }
}
static void select_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  select_state = 0;
  text_layer_set_text(text_layer, "up down");
  select_button_timer = app_timer_register(500, select_button_timer_callback, NULL);
}
static void select_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_cancel(select_button_timer);
  switch(select_state) {
    case 0:
      text_layer_set_text(text_layer, "OK");

      game_sum += curr_number * curr_modifier;
      game_round++;
      set_text(s_points_sum,"%d", game_sum);
      set_text(s_points_togo, "%d", game_start - game_sum);
      set_text(s_game_round, "Round: %d", game_round);

      curr_number = 0;
      refresh_number(0);
      curr_modifier = 1;
      break;
    case 1:
      text_layer_set_text(text_layer, "DOUBLE");
      curr_modifier = 2;
      break;
    case 2:
      text_layer_set_text(text_layer, "TRIPLE");
      curr_modifier = 3;
      break;
    case 3:
      text_layer_set_text(text_layer, "CANCEL");
      curr_modifier = 1;
      curr_number = 0;
      refresh_number(0);
      break;
  }
}





static void click_config_provider(void *context) {
  window_raw_click_subscribe(BUTTON_ID_UP, up_button_down_handler, up_button_up_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN, down_button_down_handler, down_button_up_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_SELECT, select_button_down_handler, select_button_up_handler, NULL);
}

static void window_load(Window *window) {
  // game: 301
  game_sum = 0;
  game_round = 1;
  game_start = 301;
  curr_modifier = 1;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer_number = text_layer_create((GRect) { .origin = { 2, 94 }, .size = { bounds.size.w - 4, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(text_layer_number));

  TextLayer *t_draw;

  t_draw = text_layer_create((GRect) { .origin = { 2, 2 }, .size = { 50, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  text_layer_set_text(t_draw, "Points:");

  text_points_sum = text_layer_create((GRect) { .origin = { 50, 2 }, .size = { 20, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(text_points_sum));
  set_text_layer_d(text_points_sum, s_points_sum,"%d", 0);

  t_draw = text_layer_create((GRect) { .origin = { 70, 2 }, .size = { 50, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  text_layer_set_text(t_draw, "To go:");

  text_points_togo = text_layer_create((GRect) { .origin = { 120, 2 }, .size = { 20, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(text_points_togo));
  set_text_layer_d(text_points_togo, s_points_togo, "%d", 301);

  t_draw = text_layer_create((GRect) { .origin = { 2, 22 }, .size = { 120, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  //text_layer_set_text(t_draw, "Round: 0");
  set_text_layer_d(t_draw, s_game_round, "Round: %d", game_round);


}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  curr_number = 0;

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
