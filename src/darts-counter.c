#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer_number;

static TextLayer *text_points_sum;
static TextLayer *text_points_togo;

typedef struct {
  uint8_t number; // 0: missed, 1..20, 25: bull
  uint8_t modifier; // 1: simple, 2: double, 3: tripple

} OneThrow;

// TODO: this will be configured in the app
#define NUM_OF_PLAYERS 2
#define NUM_OF_MAX_ROUNDS_IN_GAME 30

typedef struct {
  char initial;    // initial of the player
  bool isMe;    // is this player the owner of the watch? (plan to use it for online score keeping)
  uint8_t currentThrow; // starts at 0
  uint16_t thrownSum;
  OneThrow throws[NUM_OF_MAX_ROUNDS_IN_GAME][3];
} GamePlayer;

typedef struct {
  uint8_t numOfPlayers;
  uint8_t currentPlayer; // starts at 0
  uint8_t currentRound; // starts at 0
  GamePlayer players[NUM_OF_PLAYERS];
  uint16_t goalNumber; // 301, 501...
} Game;

static OneThrow currThrow;
static Game game;

static int m1;
static int m2;
static char s_s1[32];
static char s_points_sum[32];
static char s_points_togo[32];
static char s_game_round[12];
static char s_game_throw[12];
static char s_game_player[12];

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
  currThrow.number += curr;
  if (currThrow.number > 20) {
    currThrow.number = 25;
    text_layer_set_text(text_layer, "BULL");
  }
  snprintf(s_s1, sizeof(s_s1), "%d (%d)", currThrow.number, curr);
  text_layer_set_text(text_layer_number, s_s1);
}

void set_text_layer_d(TextLayer *layer, char *s, int sizeofs, char *format, int d) {
  snprintf(s, sizeofs, format, d);
  text_layer_set_text(layer, s);
}

static void set_text(char *s, int sizeofs, char *format, int d) {
  snprintf(s, sizeofs, format, d);
}

static void set_text4(char *s, int sizeofs, char *format, int d1, int d2, int d3, int d4) {
  snprintf(s, sizeofs, format, d1, d2, d3, d4);
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
  GamePlayer *currentPlayer;
  app_timer_cancel(select_button_timer);
  switch(select_state) {
    case 0:
      text_layer_set_text(text_layer, "OK");

      currentPlayer = &game.players[game.currentPlayer];
      currentPlayer->throws[game.currentRound][currentPlayer->currentThrow].number = currThrow.number;
      currentPlayer->throws[game.currentRound][currentPlayer->currentThrow].modifier = currThrow.modifier;
      currentPlayer->thrownSum += currThrow.number * currThrow.modifier;

      APP_LOG(APP_LOG_LEVEL_DEBUG, "currentPlayer: %d", game.currentPlayer);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "currentThrow before: %d", currentPlayer->currentThrow);
      currentPlayer->currentThrow = (currentPlayer->currentThrow + 1) % 3;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "currentThrow: %d", currentPlayer->currentThrow);
      if (currentPlayer->currentThrow % 3 == 0) {
        game.currentPlayer = (game.currentPlayer + 1) % game.numOfPlayers;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "currentPlayer: %d", game.currentPlayer);
        if (game.currentPlayer == 0) {
          game.currentRound++;
        }
      }

      set_text(s_game_throw, sizeof(s_game_throw), "Throw: %d", currentPlayer->currentThrow + 1);
      set_text(s_game_round, sizeof(s_game_round), "Round: %d", game.currentRound + 1);
      set_text(s_game_player, sizeof(s_game_player), "Player: %d", game.currentPlayer + 1);
      set_text4(s_points_sum, sizeof(s_points_sum), "P: [%03d] [%03d] [%03d] [%03d]",
          game.players[0].thrownSum, game.players[1].thrownSum, 0, 0);
      set_text4(s_points_togo, sizeof(s_points_togo), "G: [%03d] [%03d] [%03d] [%03d]",
          game.goalNumber - game.players[0].thrownSum, game.goalNumber - game.players[1].thrownSum, 0, 0);

      currThrow.number = 0;
      refresh_number(0);
      currThrow.modifier = 1;
      break;
    case 1:
      text_layer_set_text(text_layer, "DOUBLE");
      currThrow.modifier = 2;
      break;
    case 2:
      text_layer_set_text(text_layer, "TRIPLE");
      currThrow.modifier = 3;
      break;
    case 3:
      text_layer_set_text(text_layer, "CANCEL");
      currThrow.modifier = 1;
      currThrow.number = 0;
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "window_load, reset game data ------");
  game.numOfPlayers = NUM_OF_PLAYERS;
  game.currentPlayer = 0;
  game.currentRound = 0;
  game.goalNumber = 301;

  for(int i = 0; i < game.numOfPlayers; i++) {
    game.players[i].currentThrow = 0;
    game.players[i].thrownSum = 0;
    memset(game.players[i].throws, 0, NUM_OF_MAX_ROUNDS_IN_GAME * 3 * sizeof(OneThrow));
  }
  currThrow.modifier = 1;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 92 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  text_layer_number = text_layer_create((GRect) { .origin = { 2, 114 }, .size = { bounds.size.w - 4, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(text_layer_number));

  TextLayer *t_draw;

  t_draw = text_layer_create((GRect) { .origin = { 2, 2 }, .size = { 70, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  set_text_layer_d(t_draw, s_game_player, sizeof(s_game_player), "Player: %d", 1);

  t_draw = text_layer_create((GRect) { .origin = { 2, 22 }, .size = { 70, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  set_text_layer_d(t_draw, s_game_throw, sizeof(s_game_throw), "Throw: %d", 1);

  t_draw = text_layer_create((GRect) { .origin = { 72, 22 }, .size = { 70, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  set_text_layer_d(t_draw, s_game_round, sizeof(s_game_round), "Round: %d", 1);

  t_draw = text_layer_create((GRect) { .origin = { 2, 42 }, .size = { 140, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  set_text_layer_d(t_draw, s_points_sum, sizeof(s_points_sum), "", 0);

  t_draw = text_layer_create((GRect) { .origin = { 2, 62 }, .size = { 140, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(t_draw));
  set_text_layer_d(t_draw, s_points_togo, sizeof(s_points_togo), "", 0);

}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  currThrow.number = 0;
  currThrow.modifier = 1;
  game.numOfPlayers = NUM_OF_PLAYERS;
  game.currentPlayer = 0;

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
