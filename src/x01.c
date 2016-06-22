#include <pebble.h>
#include <x01.h>


// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_18_bold;
static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static GFont s_res_gothic_18;
static GFont s_res_gothic_14;
static TextLayer *prev_round[HISTORY_SIZE];
static TextLayer *curr_round_sum;
static TextLayer *curr_round[3];
static TextLayer *pl[4];
static TextLayer *game_name;
static TextLayer *game_round_label;
static TextLayer *game_settings;
static TextLayer *game_round_value;
static TextLayer *middle_button_text;


static char t_prev_round[HISTORY_SIZE][4];
static char t_curr_round_sum[4];
static char t_curr_round[3][4];
static char t_pl[4][5];
static char t_game_name[32];
static char t_game_settings[32];
static char t_game_round_value[4];


static OneThrow currThrow;
static Game *game;

static AppTimer *up_button_timer;
static AppTimer *down_button_timer;
static AppTimer *select_button_timer;

uint8_t up_button_timer_count = 0;
uint8_t down_button_timer_count = 0;
uint8_t select_button_timer_count = 0;

static int select_state = -1;








static void send_int(int key, int value) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "iter hiba");
      return;
    }

  dict_write_int(iter, key, &value, sizeof(int), true);
  dict_write_end(iter);
  app_message_outbox_send();
}

static void send_game() {
  GamePlayer *currentPlayer;
  char *str_message;
  char str_tmp[8] = "";


  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "iter hiba");
      return;
  }

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "malloc str_message");
  str_message = malloc(1024);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "malloc utan");
  for(int i = 0; i < game->numOfPlayers; i++) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "for 1");
    currentPlayer = &game->players[i];
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "for 2");
    str_message[0] = '\0';
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "for 3");
    for(int j = 0; j < game->currentRound + 1; j++) {
      for(int x = 0; x < 3; x++) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "snprintf elott i: %d j: %d x: %d", i, j, x);
        snprintf(str_tmp, sizeof(str_tmp), "%d,%d", currentPlayer->throws[j][x].number, currentPlayer->throws[j][x].modifier);
        str_message = strcat(str_message, str_tmp);
        if (x < 2) {
          str_message = strcat(str_message, "|");
        }
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "i: %d j: %d x: %d str_message: %s", i, j, x, str_message);
      }
      str_message = strcat(str_message, "#");
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "str_message: %s", str_message);

    }
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "dict_write_cstring: i: %d s: %s", i, str_message);
    dict_write_cstring(iter, i, str_message);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "dict_write utan");
  }
  dict_write_end(iter);
  app_message_outbox_send();

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "free elott");
  free(str_message);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "free utan");

}










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

void set_text_layer_d(TextLayer *layer, char *s, int sizeofs, char *format, int d) {
  snprintf(s, sizeofs, format, d);
  text_layer_set_text(layer, s);
}

static void refresh_number() {
  uint8_t throwNum = game->players[game->currentPlayer].currentThrow;

  if (currThrow.number > 20) {
    currThrow.number = 25;
    strcpy(t_curr_round[throwNum], " BU");
  } else {
    snprintf(t_curr_round[throwNum], sizeof(t_curr_round[throwNum]), " %d", currThrow.number);
  }
  switch(currThrow.modifier) {
    case 2: t_curr_round[throwNum][0] = 'd'; break;
    case 3: t_curr_round[throwNum][0] = 't'; break;
  }
  text_layer_set_text(curr_round[throwNum], t_curr_round[throwNum]);

  GamePlayer *currentPlayer = &game->players[game->currentPlayer];
  // before the first throw, clear out the bottom part
  int sum = 0;
  for(int x = 0; x < 3; x++) {
    sum += currentPlayer->throws[game->currentRound][x].number * currentPlayer->throws[game->currentRound][x].modifier;
  }
  sum += currThrow.number * currThrow.modifier;
  set_text_layer_d(curr_round_sum, t_curr_round_sum, sizeof(t_curr_round_sum), "%d", sum);

}

// UP BUTTON ****************************************************
static void up_button_timer_callback(void *data) {
  if (up_button_timer_count <6) {
    up_button_timer_count++;
    currThrow.number += 5;
    refresh_number();
    vibes_veryshort_pulse();
    up_button_timer = app_timer_register(500, up_button_timer_callback, NULL);
  } else {
    app_timer_cancel(up_button_timer);
  }
}
static void up_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  currThrow.number += 5;
  refresh_number();
  vibes_veryshort_pulse();
  up_button_timer_count = 0;
  up_button_timer = app_timer_register(500, up_button_timer_callback, NULL);
}
static void up_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_cancel(up_button_timer);
  //text_layer_set_text(text_layer, "Up: +5");
}

// DOWN BUTTON ****************************************************
static void down_button_timer_callback(void *data) {
  if (down_button_timer_count <10) {
    down_button_timer_count++;
    currThrow.number += 1;
    refresh_number();
    vibes_veryshort_pulse();
    down_button_timer = app_timer_register(500, down_button_timer_callback, NULL);
  } else {
    app_timer_cancel(down_button_timer);
  }
}
static void down_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  currThrow.number += 1;
  refresh_number();
  vibes_veryshort_pulse();
  down_button_timer_count = 0;
  down_button_timer = app_timer_register(500, down_button_timer_callback, NULL);
}
static void down_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  app_timer_cancel(down_button_timer);
  //text_layer_set_text(text_layer, "Down: +1");
}

// SELECT BUTTON ****************************************************
static void select_button_timer_callback(void *data) {
  if (select_button_timer_count <4) {
    select_button_timer_count++;
    select_state++;
    vibes_veryshort_number(select_state + 1);
    if (select_state < 3) {
      select_button_timer = app_timer_register(500, select_button_timer_callback, NULL);
    }
    switch(select_state) {
      case 1: text_layer_set_text(middle_button_text, "DOUBLE"); break;
      case 2: text_layer_set_text(middle_button_text, "TRIPLE"); break;
      case 3: text_layer_set_text(middle_button_text, "CANCEL"); break;
    }
  } else {
    app_timer_cancel(select_button_timer);
  }
}
static void select_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  select_state = 0;
  //text_layer_set_text(text_layer, "up down");
  select_button_timer_count = 0;
  select_button_timer = app_timer_register(500, select_button_timer_callback, NULL);
}
static void select_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  GamePlayer *currentPlayer;
  app_timer_cancel(select_button_timer);
  switch(select_state) {
    case 0:
      // OK pressed

      currentPlayer = &game->players[game->currentPlayer];

      text_layer_set_text(middle_button_text, "");
      text_layer_set_text(curr_round_sum, "");

      currentPlayer->throws[game->currentRound][currentPlayer->currentThrow].number = currThrow.number;
      currentPlayer->throws[game->currentRound][currentPlayer->currentThrow].modifier = currThrow.modifier;
      currentPlayer->thrownSum += currThrow.number * currThrow.modifier;

      APP_LOG(APP_LOG_LEVEL_DEBUG, "currentPlayer: %d", game->currentPlayer);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "currentThrow before: %d", currentPlayer->currentThrow);
      currentPlayer->currentThrow = (currentPlayer->currentThrow + 1) % 3;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "currentThrow: %d game goal: %d", currentPlayer->currentThrow, game->goalNumber);
      bool nextPlayer = false;
      bool resetRound = false;
      if (currentPlayer->currentThrow % 3 == 0) {
        nextPlayer = true;
      }
      if (currentPlayer->thrownSum > game->goalNumber) {
        // too much
        nextPlayer = true;
        resetRound = true;
      }
      if (game->isDoubleOut
          && ((currentPlayer->thrownSum == game->goalNumber && currThrow.modifier != 2)
                || currentPlayer->thrownSum == game->goalNumber - 1
             )
        ) {
        // goal reached, but not with double or 1 is the result
        nextPlayer = true;
        resetRound = true;
      }
      if (nextPlayer) {
        if (resetRound) {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "TOOOO MUCH");
          // too much, reset current round
          for (int j = 0; j < 3; j++) {
            currentPlayer->thrownSum -=  currentPlayer->throws[game->currentRound][j].number * currentPlayer->throws[game->currentRound][j].modifier;
            currentPlayer->throws[game->currentRound][j].number = 0;
            currentPlayer->throws[game->currentRound][j].modifier = 1;
            currentPlayer->currentThrow = 0;
          }
        }
        // update the score of the latest player
        set_text_layer_d(pl[game->currentPlayer], t_pl[game->currentPlayer], sizeof(t_pl[game->currentPlayer]), "%d", game->goalNumber - game->players[game->currentPlayer].thrownSum);

        // next player
        text_layer_set_background_color(pl[game->currentPlayer], GColorBlack);
        text_layer_set_text_color(pl[game->currentPlayer], GColorWhite);

        game->currentPlayer = (game->currentPlayer + 1) % game->numOfPlayers;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "currentPlayer: %d", game->currentPlayer);

        text_layer_set_background_color(pl[game->currentPlayer], GColorWhite);
        text_layer_set_text_color(pl[game->currentPlayer], GColorBlack);

        if (game->currentPlayer == 0) {
          // next round
          game->currentRound++;
        }

        currentPlayer = &game->players[game->currentPlayer];
        // before the first throw, clear out the bottom part
        for(int i = 0; i < 3; i++) {
          set_text_layer_d(curr_round[i], t_curr_round[i], sizeof(t_curr_round[i]), "", 0);
        }

        int histCount = 0;
        for(int i = game->currentRound -1; i >= 0 && histCount < HISTORY_SIZE; i--) {
          int sum = 0;
          for(int x = 0; x < 3; x++) {
            sum += currentPlayer->throws[i][x].number * currentPlayer->throws[i][x].modifier;
          }
          set_text_layer_d(prev_round[histCount], t_prev_round[histCount], sizeof(t_prev_round[histCount]), "%d", sum);
          histCount++;
        }

      }

      set_text_layer_d(pl[game->currentPlayer], t_pl[game->currentPlayer], sizeof(t_pl[game->currentPlayer]), "%d", game->goalNumber - game->players[game->currentPlayer].thrownSum);
      set_text_layer_d(game_round_value, t_game_round_value, sizeof(t_game_round_value), "%d", game->currentRound + 1);

      currentPlayer = &game->players[game->currentPlayer];
      set_text_layer_d(curr_round[currentPlayer->currentThrow],
        t_curr_round[currentPlayer->currentThrow],
        sizeof(t_curr_round[currentPlayer->currentThrow]), "%d", 0);

      currThrow.number = 0;
      currThrow.modifier = 1;
      send_game();
      //refresh_number();
      break;
    case 1:
      //text_layer_set_text(text_layer, "DOUBLE");
      currThrow.modifier = 2;
      refresh_number();
      break;
    case 2:
      //text_layer_set_text(text_layer, "TRIPLE");
      currThrow.modifier = 3;
      refresh_number();
      break;
    case 3:
      //text_layer_set_text(text_layer, "CANCEL");
      currThrow.modifier = 1;
      currThrow.number = 0;
      refresh_number();
      break;
  }
}

// SELECT BUTTON ****************************************************



static void click_config_provider(void *context) {
  window_raw_click_subscribe(BUTTON_ID_UP, up_button_down_handler, up_button_up_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN, down_button_down_handler, down_button_up_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_SELECT, select_button_down_handler, select_button_up_handler, NULL);
}

static void reset_game() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "window_load, reset game data ------");
  game->currentPlayer = 0;
  game->currentRound = 0;

  for(int i = 0; i < game->numOfPlayers; i++) {
    game->players[i].currentThrow = 0;
    game->players[i].thrownSum = 0;
    memset(game->players[i].throws, 0, NUM_OF_MAX_ROUNDS_IN_GAME * 3 * sizeof(OneThrow));

    set_text_layer_d(pl[i], t_pl[i], sizeof(t_pl[i]), "%d", game->goalNumber);
  }
  set_text_layer_d(curr_round[0], t_curr_round[0], sizeof(t_curr_round[0]), "%d", 0);
  set_text_layer_d(game_name, t_game_name, sizeof(t_game_name), "Game: %d", game->goalNumber);
  char s1[4] = "Yes";
  char s2[4] = "Yes";
  if (!game->isDoubleIn) { strcpy(s1, "No"); }
  if (!game->isDoubleOut) { strcpy(s2, "No"); }
  snprintf(t_game_settings, sizeof(t_game_settings), "Double in: %s out: %s", s1, s2);
  text_layer_set_text(game_settings, t_game_settings);

  currThrow.modifier = 1;

}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, false);
  #endif

  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  // prev_round[3]
  prev_round[3] = text_layer_create(GRect(2, 131, 25, 22));
  text_layer_set_text(prev_round[3], "");
  text_layer_set_text_alignment(prev_round[3], GTextAlignmentCenter);
  text_layer_set_font(prev_round[3], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[3]);

  // prev_round[2]
  prev_round[2] = text_layer_create(GRect(29, 131, 25, 22));
  text_layer_set_text(prev_round[2], "");
  text_layer_set_text_alignment(prev_round[2], GTextAlignmentCenter);
  text_layer_set_font(prev_round[2], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[2]);

  // prev_round[1]
  prev_round[1] = text_layer_create(GRect(56, 131, 25, 22));
  text_layer_set_text(prev_round[1], "");
  text_layer_set_text_alignment(prev_round[1], GTextAlignmentCenter);
  text_layer_set_font(prev_round[1], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[1]);

  // prev_round[0]
  prev_round[0] = text_layer_create(GRect(83, 131, 25, 22));
  text_layer_set_text(prev_round[0], "");
  text_layer_set_text_alignment(prev_round[0], GTextAlignmentCenter);
  text_layer_set_font(prev_round[0], s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)prev_round[0]);

  // curr_round_sum
  curr_round_sum = text_layer_create(GRect(110, 130, 32, 24));
  text_layer_set_text(curr_round_sum, "");
  text_layer_set_text_alignment(curr_round_sum, GTextAlignmentCenter);
  text_layer_set_font(curr_round_sum, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round_sum);

  // curr_round[0]
  curr_round[0] = text_layer_create(GRect(2, 93, 45, 29));
  text_layer_set_text(curr_round[0], "");
  text_layer_set_text_alignment(curr_round[0], GTextAlignmentCenter);
  text_layer_set_font(curr_round[0], s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round[0]);

  // curr_round[1]
  curr_round[1] = text_layer_create(GRect(49, 93, 46, 29));
  text_layer_set_text(curr_round[1], "");
  text_layer_set_text_alignment(curr_round[1], GTextAlignmentCenter);
  text_layer_set_font(curr_round[1], s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round[1]);

  // curr_round[2]
  curr_round[2] = text_layer_create(GRect(97, 93, 45, 29));
  text_layer_set_text(curr_round[2], "");
  text_layer_set_text_alignment(curr_round[2], GTextAlignmentCenter);
  text_layer_set_font(curr_round[2], s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)curr_round[2]);

  // pl[0]
  pl[0] = text_layer_create(GRect(2, 65, 34, 28));
  text_layer_set_background_color(pl[0], GColorWhite);
  text_layer_set_text_color(pl[0], GColorBlack);
  text_layer_set_text(pl[0], "");
  text_layer_set_text_alignment(pl[0], GTextAlignmentCenter);
  text_layer_set_font(pl[0], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[0]);

  // pl[1]
  pl[1] = text_layer_create(GRect(37, 65, 35, 28));
  text_layer_set_background_color(pl[1], GColorBlack);
  text_layer_set_text_color(pl[1], GColorWhite);
  text_layer_set_text(pl[1], "");
  text_layer_set_text_alignment(pl[1], GTextAlignmentCenter);
  text_layer_set_font(pl[1], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[1]);

  // pl[2]
  pl[2] = text_layer_create(GRect(73, 65, 34, 28));
  text_layer_set_background_color(pl[2], GColorBlack);
  text_layer_set_text_color(pl[2], GColorWhite);
  text_layer_set_text(pl[2], "");
  text_layer_set_text_alignment(pl[2], GTextAlignmentCenter);
  text_layer_set_font(pl[2], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[2]);

  // pl[3]
  pl[3] = text_layer_create(GRect(108, 65, 34, 28));
  text_layer_set_background_color(pl[3], GColorBlack);
  text_layer_set_text_color(pl[3], GColorWhite);
  text_layer_set_text(pl[3], "");
  text_layer_set_text_alignment(pl[3], GTextAlignmentCenter);
  text_layer_set_font(pl[3], s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)pl[3]);

  // game_name
  game_name = text_layer_create(GRect(5, 4, 63, 20));
  text_layer_set_background_color(game_name, GColorClear);
  text_layer_set_text_color(game_name, GColorWhite);
  text_layer_set_text(game_name, "Game: ");
  text_layer_set_font(game_name, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_name);

  // game_round_label
  game_round_label = text_layer_create(GRect(76, 4, 40, 20));
  text_layer_set_background_color(game_round_label, GColorClear);
  text_layer_set_text_color(game_round_label, GColorWhite);
  text_layer_set_text(game_round_label, "Round:");
  text_layer_set_font(game_round_label, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_round_label);

  // game_settings
  game_settings = text_layer_create(GRect(8, 27, 132, 20));
  text_layer_set_background_color(game_settings, GColorClear);
  text_layer_set_text_color(game_settings, GColorWhite);
  text_layer_set_text(game_settings, "");
  text_layer_set_font(game_settings, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_settings);

  // game_round_value
  game_round_value = text_layer_create(GRect(115, 1, 27, 24));
  text_layer_set_background_color(game_round_value, GColorClear);
  text_layer_set_text_color(game_round_value, GColorWhite);
  text_layer_set_text(game_round_value, "22");
  text_layer_set_text_alignment(game_round_value, GTextAlignmentCenter);
  text_layer_set_font(game_round_value, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)game_round_value);

  // middle_button_text
  middle_button_text = text_layer_create(GRect(82, 50, 59, 16));
  text_layer_set_background_color(middle_button_text, GColorClear);
  text_layer_set_text_color(middle_button_text, GColorWhite);
  text_layer_set_text(middle_button_text, "");
  text_layer_set_text_alignment(middle_button_text, GTextAlignmentRight);
  text_layer_set_font(middle_button_text, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)middle_button_text);

}

static void destroy_ui(void) {
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
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void hide_window_ui(void) {
  window_stack_remove(s_window, true);
}

void x01_window_push(Game *pgame) {
  currThrow.number = 0;
  currThrow.modifier = 1;
  game = pgame;
  game->currentPlayer = 0;

  s_window = window_create();
  initialise_ui();
  reset_game();
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}
