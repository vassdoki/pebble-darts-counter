#include <pebble.h>
#include "x01_result.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_14;
static GFont s_res_gothic_24_bold;
static TextLayer *s_tl_bottom_text;
static TextLayer *s_tl_stats;
static TextLayer *s_tl_stats_header;
static TextLayer *s_tl_stat_3;
static TextLayer *s_tl_stat_2;
static TextLayer *s_tl_stat_1;
static TextLayer *s_tl_stat_4;
static TextLayer *s_tl_winner;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  // s_tl_bottom_text
  s_tl_bottom_text = text_layer_create(GRect(7, 145, 127, 17));
  text_layer_set_text(s_tl_bottom_text, "Press back to play again");
  text_layer_set_font(s_tl_bottom_text, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_bottom_text);
  
  // s_tl_stats
  s_tl_stats = text_layer_create(GRect(4, 42, 117, 16));
  text_layer_set_text(s_tl_stats, "Stats   Rounds: 12");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_stats);
  
  // s_tl_stats_header
  s_tl_stats_header = text_layer_create(GRect(4, 60, 134, 16));
  text_layer_set_text(s_tl_stats_header, "             Avg     Max     Min");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_stats_header);
  
  // s_tl_stat_3
  s_tl_stat_3 = text_layer_create(GRect(4, 95, 135, 16));
  text_layer_set_text(s_tl_stat_3, "Pl 2       123      123     123");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_stat_3);
  
  // s_tl_stat_2
  s_tl_stat_2 = text_layer_create(GRect(4, 110, 135, 16));
  text_layer_set_text(s_tl_stat_2, "Pl 3       123      123     123");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_stat_2);
  
  // s_tl_stat_1
  s_tl_stat_1 = text_layer_create(GRect(4, 80, 135, 16));
  text_layer_set_text(s_tl_stat_1, "Pl 1       123      123     123");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_stat_1);
  
  // s_tl_stat_4
  s_tl_stat_4 = text_layer_create(GRect(4, 125, 135, 16));
  text_layer_set_text(s_tl_stat_4, "Pl 3       123      123     123");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_stat_4);
  
  // s_tl_winner
  s_tl_winner = text_layer_create(GRect(4, 9, 136, 24));
  text_layer_set_text(s_tl_winner, "Winner: Player #1");
  text_layer_set_font(s_tl_winner, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tl_winner);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_tl_bottom_text);
  text_layer_destroy(s_tl_stats);
  text_layer_destroy(s_tl_stats_header);
  text_layer_destroy(s_tl_stat_3);
  text_layer_destroy(s_tl_stat_2);
  text_layer_destroy(s_tl_stat_1);
  text_layer_destroy(s_tl_stat_4);
  text_layer_destroy(s_tl_winner);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_x01_result(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_x01_result(void) {
  window_stack_remove(s_window, true);
}
