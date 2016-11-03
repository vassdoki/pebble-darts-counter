#include <pebble.h>
#include "x01_gui.h"
#include "util_gui.h"

static Window *s_window;
static StatusBarLayer *s_status_bar;

static GFont s_res_gothic_18_bold;
static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_18;
static GFont s_res_gothic_14;

static char t_game_settings[32];

static char t_prev_round[HISTORY_SIZE][4];
static char t_curr_round_sum[4];
static char t_pl[4][5];
static char t_game_name[32];
static char t_game_round_value[4];

static void setup_window(void);
static void handle_window_unload(Window* window);

void x01_gui_window_push(ClickConfigProvider click_config_provider) {
  s_window = window_create();
  setup_window();
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void handle_window_unload(Window* window) {
  x01_gui_destroy_ui();
}


void x01_gui_hide_window_ui(void) {
  window_stack_remove(s_window, true);
}

static TextLayer *prev_round[HISTORY_SIZE];
void x01_gui_draw_prev_round(int round, int value) {
  set_text_layer_d(prev_round[round], t_prev_round[round], sizeof(t_prev_round[0]), "%d", value);
}

static TextLayer *curr_round_sum;
void x01_gui_draw_curr_round(int value) {
  set_text_layer_d(curr_round_sum, t_curr_round_sum, sizeof(t_curr_round_sum), "%d", value);
}
void x01_gui_draw_curr_round_clear() {
  text_layer_set_text(curr_round_sum, "");
}

static TextLayer *curr_round[3];
void x01_gui_draw_throw(int count, char* value) {
  text_layer_set_text(curr_round[count], value);
}
void x01_gui_draw_throw_clear(int count) {
  text_layer_set_text(curr_round[count], "");
}
void x01_gui_draw_throw_clear_all() {
  for(int i = 0; i < 3; i++) {
     x01_gui_draw_throw_clear(i);
  }
}

static TextLayer *pl[4];
void x01_gui_draw_player_score(int player, int score) {
  set_text_layer_d(pl[player], t_pl[player], sizeof(t_pl[0]), "%d", score);
}

static TextLayer *game_round_label;
static TextLayer *game_round_value;
void x01_gui_draw_game_round(int number) {
  set_text_layer_d(game_round_value, t_game_round_value, sizeof(t_game_round_value), "%d", number);
}

static TextLayer *game_name;
void x01_gui_draw_game_name(int number) {
  set_text_layer_d(game_name, t_game_name, sizeof(t_game_name), "Game: %d", number);
}

static TextLayer *game_settings;
void x01_gui_draw_game_settings(char* s) {
  text_layer_set_text(game_settings, s);
}
void x01_gui_draw_game_settings_2b(bool isDoubleIn, bool isDoubleOut) {
  char s1[4] = "Yes";
  char s2[4] = "Yes";
  if (!isDoubleIn) { strcpy(s1, "No"); }
  if (!isDoubleOut) { strcpy(s2, "No"); }
  snprintf(t_game_settings, sizeof(t_game_settings), "Double in: %s out: %s", s1, s2);
  text_layer_set_text(game_settings, t_game_settings);
}


static TextLayer *middle_button_text;
void x01_gui_draw_status(char* s) {
  text_layer_set_text(middle_button_text, s);
}

void x01_gui_player_on(int i) {
  text_layer_set_background_color(pl[i], GColorWhite);
  text_layer_set_text_color(pl[i], GColorBlack);
}
void x01_gui_player_off(int i) {
  text_layer_set_background_color(pl[i], GColorBlack);
  text_layer_set_text_color(pl[i], GColorWhite);
}

// ***************************************************************************

void x01_gui_destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(prev_round[3]);
  text_layer_destroy(prev_round[2]);
  text_layer_destroy(prev_round[1]);
  text_layer_destroy(prev_round[0]);
  text_layer_destroy(curr_round_sum);
  text_layer_destroy(curr_round[0]);
  text_layer_destroy(curr_round[1]);
  text_layer_destroy(curr_round[2]);
  text_layer_destroy(pl[0]);
  text_layer_destroy(pl[1]);
  text_layer_destroy(pl[2]);
  text_layer_destroy(pl[3]);
  text_layer_destroy(game_name);
  text_layer_destroy(game_round_label);
  text_layer_destroy(game_settings);
  text_layer_destroy(game_round_value);
  status_bar_layer_destroy(s_status_bar);
}

static void setup_window(void) {
  int xofs = 0;
  int yofs = 0;
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);

  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);

  // status bar with the time (this is a watch after all)
  s_status_bar = status_bar_layer_create();
  status_bar_layer_set_colors(s_status_bar, GColorBlack, GColorWhite);
  status_bar_layer_set_separator_mode(s_status_bar, StatusBarLayerSeparatorModeDotted);
  layer_add_child(window_get_root_layer(s_window), status_bar_layer_get_layer(s_status_bar));

  if (PBL_DISPLAY_WIDTH > 144) {
      xofs = (PBL_DISPLAY_WIDTH - 144) / 2;
  }
  yofs = STATUS_BAR_LAYER_HEIGHT - 16;

  // game_name
  game_name = text_layer_create(GRect(xofs + 5, yofs + 11, 63, 20));
  text_layer_set_background_color(game_name, GColorClear);
  text_layer_set_text_color(game_name, GColorWhite);
  text_layer_set_text(game_name, "Game: ");
  text_layer_set_font(game_name, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_name);

  // game_round_label
  game_round_label = text_layer_create(GRect(xofs + 76, yofs + 11, 40, 20));
  text_layer_set_background_color(game_round_label, GColorClear);
  text_layer_set_text_color(game_round_label, GColorWhite);
  text_layer_set_text(game_round_label, "Round:");
  text_layer_set_font(game_round_label, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_round_label);

  // game_round_value
  game_round_value = text_layer_create(GRect(xofs + 115, yofs + 8, 27, 24));
  text_layer_set_background_color(game_round_value, GColorClear);
  text_layer_set_text_color(game_round_value, GColorWhite);
  text_layer_set_text(game_round_value, "0");
  text_layer_set_text_alignment(game_round_value, GTextAlignmentCenter);
  text_layer_set_font(game_round_value, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_round_value);

  // game_settings
  game_settings = text_layer_create(GRect(xofs + 8, yofs + 33, 132, 20));
  text_layer_set_background_color(game_settings, GColorClear);
  text_layer_set_text_color(game_settings, GColorWhite);
  text_layer_set_text(game_settings, "");
  text_layer_set_font(game_settings, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_settings);

  // middle_button_text
  middle_button_text = text_layer_create(GRect(xofs + 5, yofs + 49, 136, 16));
  text_layer_set_background_color(middle_button_text, GColorClear);
  text_layer_set_text_color(middle_button_text, GColorWhite);
  text_layer_set_text(middle_button_text, "");
  text_layer_set_text_alignment(middle_button_text, GTextAlignmentRight);
  text_layer_set_font(middle_button_text, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)middle_button_text);

  // pl[0]
  pl[0] = text_layer_create(GRect(xofs + 2, yofs + 65, 34, 28));
  text_layer_set_background_color(pl[0], GColorWhite);
  text_layer_set_text_color(pl[0], GColorBlack);
  text_layer_set_text(pl[0], "");
  text_layer_set_text_alignment(pl[0], GTextAlignmentCenter);
  text_layer_set_font(pl[0], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[0]);

  // pl[1]
  pl[1] = text_layer_create(GRect(xofs + 37, yofs + 65, 35, 28));
  text_layer_set_background_color(pl[1], GColorBlack);
  text_layer_set_text_color(pl[1], GColorWhite);
  text_layer_set_text(pl[1], "");
  text_layer_set_text_alignment(pl[1], GTextAlignmentCenter);
  text_layer_set_font(pl[1], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[1]);

  // pl[2]
  pl[2] = text_layer_create(GRect(xofs + 73, yofs + 65, 34, 28));
  text_layer_set_background_color(pl[2], GColorBlack);
  text_layer_set_text_color(pl[2], GColorWhite);
  text_layer_set_text(pl[2], "");
  text_layer_set_text_alignment(pl[2], GTextAlignmentCenter);
  text_layer_set_font(pl[2], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[2]);

  // pl[3]
  pl[3] = text_layer_create(GRect(xofs + 108, yofs + 65, 34, 28));
  text_layer_set_background_color(pl[3], GColorBlack);
  text_layer_set_text_color(pl[3], GColorWhite);
  text_layer_set_text(pl[3], "");
  text_layer_set_text_alignment(pl[3], GTextAlignmentCenter);
  text_layer_set_font(pl[3], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[3]);

  // curr_round[0]
  curr_round[0] = text_layer_create(GRect(xofs + 2, yofs + 93, 45, 29));
  text_layer_set_text(curr_round[0], "");
  text_layer_set_text_alignment(curr_round[0], GTextAlignmentCenter);
  text_layer_set_font(curr_round[0], s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round[0]);

  // curr_round[1]
  curr_round[1] = text_layer_create(GRect(xofs + 49, yofs + 93, 46, 29));
  text_layer_set_text(curr_round[1], "");
  text_layer_set_text_alignment(curr_round[1], GTextAlignmentCenter);
  text_layer_set_font(curr_round[1], s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round[1]);

  // curr_round[2]
  curr_round[2] = text_layer_create(GRect(xofs + 97, yofs + 93, 45, 29));
  text_layer_set_text(curr_round[2], "");
  text_layer_set_text_alignment(curr_round[2], GTextAlignmentCenter);
  text_layer_set_font(curr_round[2], s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round[2]);

  // prev_round[3]
  prev_round[3] = text_layer_create(GRect(xofs + 2, yofs + 131, 25, 22));
  text_layer_set_text(prev_round[3], "");
  text_layer_set_text_alignment(prev_round[3], GTextAlignmentCenter);
  text_layer_set_font(prev_round[3], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[3]);

  // prev_round[2]
  prev_round[2] = text_layer_create(GRect(xofs + 29, yofs + 131, 25, 22));
  text_layer_set_text(prev_round[2], "");
  text_layer_set_text_alignment(prev_round[2], GTextAlignmentCenter);
  text_layer_set_font(prev_round[2], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[2]);

  // prev_round[1]
  prev_round[1] = text_layer_create(GRect(xofs + 56, yofs + 131, 25, 22));
  text_layer_set_text(prev_round[1], "");
  text_layer_set_text_alignment(prev_round[1], GTextAlignmentCenter);
  text_layer_set_font(prev_round[1], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[1]);

  // prev_round[0]
  prev_round[0] = text_layer_create(GRect(xofs + 83, yofs + 131, 25, 22));
  text_layer_set_text(prev_round[0], "");
  text_layer_set_text_alignment(prev_round[0], GTextAlignmentCenter);
  text_layer_set_font(prev_round[0], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[0]);

  // curr_round_sum
  curr_round_sum = text_layer_create(GRect(xofs + 110, yofs + 130, 32, 24));
  text_layer_set_text(curr_round_sum, "");
  text_layer_set_text_alignment(curr_round_sum, GTextAlignmentCenter);
  text_layer_set_font(curr_round_sum, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round_sum);

}

