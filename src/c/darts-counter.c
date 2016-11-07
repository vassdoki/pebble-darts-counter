#include <pebble.h>
#include "config.h"
#include "x01.h"

static Window *s_main_window;
static MenuLayer *s_menu_layer;

Game *game;
static char s_text[4][32];

#define NUM_MENU 6
#define CHECKBOX_WINDOW_CELL_HEIGHT 44

#define KEY_DATA 0

static char s_buffer[64];
#if PUSH_GAME_HTTP == 1
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *data = dict_find(iterator, KEY_DATA);
  if (data) {
    snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", data->value->cstring);
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message received: %s", s_buffer);
  }
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
#endif

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  if (game->currentRound > 0) {
    return NUM_MENU + 1;
  } else {
    return NUM_MENU;
  }
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  snprintf(s_text[0], sizeof(s_text[0]), "%d", game->numOfPlayers);
  snprintf(s_text[1], sizeof(s_text[1]), "%d", game->goalNumber);
  if (game->isDoubleIn) strcpy(s_text[2], "Yes"); else strcpy(s_text[2], "No");
  if (game->isDoubleOut) strcpy(s_text[3], "Yes"); else strcpy(s_text[3], "No");
  switch(cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Number of players", s_text[0], NULL);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Game type", "x01", NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Starts from", s_text[1], NULL);
      break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "Double in", s_text[2], NULL);
      break;
    case 4:
      menu_cell_basic_draw(ctx, cell_layer, "Double out", s_text[3], NULL);
      break;
    case 5:
      menu_cell_basic_draw(ctx, cell_layer, "Start game", NULL, NULL);
      break;
    case 6:
      menu_cell_basic_draw(ctx, cell_layer, "Continue game", NULL, NULL);
      break;
    default:
      break;
  }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ?
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    CHECKBOX_WINDOW_CELL_HEIGHT);
}

// TODO: if the game settings are changed, then "continue game" menu should disappear
static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  switch(cell_index->row) {
    case 0:
      game->numOfPlayers = (game->numOfPlayers % MAX_NUM_OF_PLAYERS) + 1;
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
    case 1:
      break;
    case 2:
      game->goalNumber += 200;
      if (game->goalNumber > 701) {
        game->goalNumber = 301;
      }
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
    case 3:
      game->isDoubleIn = !game->isDoubleIn;
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
    case 4:
      game->isDoubleOut = !game->isDoubleOut;
      layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
    case 5:
      x01_window_push(game, 1);
      break;
    case 6:
      x01_window_push(game, 0);
      break;
    default:
      break;
  }
}

static void window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "WINDOW LOAD");
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
#if defined(PBL_COLOR)
  menu_layer_set_normal_colors(s_menu_layer, GColorBlack, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorRed, GColorWhite);
#endif
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_appear(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "APPEAR");
  menu_layer_reload_data(s_menu_layer);
}


static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

static void init() {
#if PUSH_GAME_HTTP == 1
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(128, 2048);
#endif

  game = malloc(sizeof *game);
  game->numOfPlayers = 1;
  game->goalNumber = 301;
  game->winner = 0;
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
      .appear = window_appear
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
