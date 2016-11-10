#include <pebble.h>
#include "x01_usage.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static TextLayer *s_textlayer_1;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // s_textlayer_1
  s_textlayer_1 = text_layer_create(GRect(2, 2, PBL_DISPLAY_WIDTH - 4, PBL_DISPLAY_HEIGHT - 4));
  text_layer_set_text(s_textlayer_1, "Usage\n\n# Up: +1\n# Up long: +19\n# Select: enter throw\n# Select long: 2x, 3x, cancel, cancel round. Release the button when you see the option.\n# Down: +5\n# Down long: +16");
  #ifdef PBL_ROUND
     text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  #endif
  text_layer_set_font(s_textlayer_1, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_x01_usage(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_x01_usage(void) {
  window_stack_remove(s_window, true);
}
