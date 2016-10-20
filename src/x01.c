#include <pebble.h>
#include "config.h"
#include "x01.h"
#include "util_gui.h"

/** The current throw of the current player */
static OneThrow currThrow;
/** Game configuration */
static Game *game;
/** Timer for the select button */
static AppTimer *select_button_timer;
uint8_t select_button_timer_count = 0;
static int select_state = -1;

/**
 * The x01 game logic is implemented here
 */
static void process_throw(void) {
  GamePlayer *currentPlayer;
  int currentThrowValue;

  currentPlayer = &game->players[game->currentPlayer];

  x01_gui_draw_status("");
  x01_gui_draw_curr_round_clear();

  currentPlayer->throws[game->currentRound][currentPlayer->currentThrow].number = currThrow.number;
  currentPlayer->throws[game->currentRound][currentPlayer->currentThrow].modifier = currThrow.modifier;

  currentThrowValue = currThrow.number * currThrow.modifier;
  currentPlayer->thrownSum += currentThrowValue;

  currentPlayer->currentThrow = (currentPlayer->currentThrow + 1) % 3;
  bool nextPlayer = false;
  bool resetRound = false;
  if (currentPlayer->currentThrow % 3 == 0) {
    nextPlayer = true;
  }
  if (currentPlayer->thrownSum > game->goalNumber) {
    // too much
    nextPlayer = true;
    resetRound = true;
    x01_gui_draw_status("Too much!");
  }
  if (game->isDoubleOut
      && ((currentPlayer->thrownSum == game->goalNumber && currThrow.modifier != 2)
            || currentPlayer->thrownSum == game->goalNumber - 1
         )
    ) {
    // goal reached, but not with double or 1 is the result
    nextPlayer = true;
    resetRound = true;
    x01_gui_draw_status("Wrong double out!");
  }
  if (game->isDoubleIn && currentPlayer->thrownSum == currentThrowValue && currThrow.modifier != 2 && currentThrowValue > 0) {
    // double in error
    nextPlayer = true;
    resetRound = true;
    x01_gui_draw_status("Wrong double in!");
  }
  if (nextPlayer) {
    if (resetRound) {
      // too much, reset current round
      for (int j = 0; j < 3; j++) {
        currentPlayer->thrownSum -=  currentPlayer->throws[game->currentRound][j].number * currentPlayer->throws[game->currentRound][j].modifier;
        currentPlayer->throws[game->currentRound][j].number = 0;
        currentPlayer->throws[game->currentRound][j].modifier = 1;
        currentPlayer->currentThrow = 0;
      }
    }
    // update the score of the latest player
    x01_gui_draw_player_score(game->currentPlayer, game->goalNumber - game->players[game->currentPlayer].thrownSum);

    // next player
    x01_gui_player_off(game->currentPlayer);
    game->currentPlayer = (game->currentPlayer + 1) % game->numOfPlayers;
    x01_gui_player_on(game->currentPlayer);
    if (game->currentPlayer == 0) {
      // next round
      game->currentRound++;
    }

    currentPlayer = &game->players[game->currentPlayer];
    // before the first throw, clear out the bottom part
    x01_gui_draw_throw_clear_all();
    //draw the history
    int histCount = 0;
    for(int i = game->currentRound -1; i >= 0 && histCount < HISTORY_SIZE; i--) {
      int sum = 0;
      for(int x = 0; x < 3; x++) {
        sum += currentPlayer->throws[i][x].number * currentPlayer->throws[i][x].modifier;
      }
      x01_gui_draw_prev_round(histCount, sum);
      histCount++;
    }
  }

  x01_gui_draw_player_score(game->currentPlayer, game->goalNumber - game->players[game->currentPlayer].thrownSum);
  x01_gui_draw_game_round(game->currentRound + 1);

  currentPlayer = &game->players[game->currentPlayer];
  x01_gui_draw_throw(currentPlayer->currentThrow, "0");

  currThrow.number = 0;
  currThrow.modifier = 1;
  #if PUSH_GAME_HTTP == 1
    send_game();
  #endif
}

static void refresh_throw(int throwNum, int number, int modifier) {
  static char t_curr_round[3][4];

  if (number > 20) {
    number = 25;
    strcpy(t_curr_round[throwNum], " BU");
    if (modifier > 2) {
      modifier = 2;
    }
  } else {
    snprintf(t_curr_round[throwNum], sizeof(t_curr_round[throwNum]), " %d", number);
  }
  switch(modifier) {
    case 2: t_curr_round[throwNum][0] = 'd'; break;
    case 3: t_curr_round[throwNum][0] = 't'; break;
  }
  x01_gui_draw_throw(throwNum, t_curr_round[throwNum]);
}

static void refresh_number() {
  static char t_curr_round[3][4];
  uint8_t throwNum = game->players[game->currentPlayer].currentThrow;

  refresh_throw(throwNum, currThrow.number, currThrow.modifier);
//  if (currThrow.number > 20) {
//    currThrow.number = 25;
//    strcpy(t_curr_round[throwNum], " BU");
//    if (currThrow.modifier > 2) {
//      currThrow.modifier = 2;
//    }
//  } else {
//    snprintf(t_curr_round[throwNum], sizeof(t_curr_round[throwNum]), " %d", currThrow.number);
//  }
//  switch(currThrow.modifier) {
//    case 2: t_curr_round[throwNum][0] = 'd'; break;
//    case 3: t_curr_round[throwNum][0] = 't'; break;
//  }
//  x01_gui_draw_throw(throwNum, t_curr_round[throwNum]);

  GamePlayer *currentPlayer = &game->players[game->currentPlayer];
  // before the first throw, clear out the bottom part
  int sum = 0;
  for(int x = 0; x < 3; x++) {
    sum += currentPlayer->throws[game->currentRound][x].number * currentPlayer->throws[game->currentRound][x].modifier;
  }
  sum += currThrow.number * currThrow.modifier;
  x01_gui_draw_curr_round(sum);
}

#if PUSH_GAME_HTTP == 1
static void send_game() {
  GamePlayer *currentPlayer;
  char *str_message;
  char str_tmp[8] = "";

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (!iter) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "iter error");
      return;
  }

  str_message = malloc(1024);
  for(int i = 0; i < game->numOfPlayers; i++) {
    currentPlayer = &game->players[i];
    str_message[0] = '\0';
    for(int j = 0; j < game->currentRound + 1; j++) {
      for(int x = 0; x < 3; x++) {
        snprintf(str_tmp, sizeof(str_tmp), "%d,%d", currentPlayer->throws[j][x].number, currentPlayer->throws[j][x].modifier);
        str_message = strcat(str_message, str_tmp);
        if (x < 2) {
          str_message = strcat(str_message, "|");
        }
      }
      str_message = strcat(str_message, "#");
    }
    dict_write_cstring(iter, i, str_message);
  }
  dict_write_end(iter);
  app_message_outbox_send();

  free(str_message);
}
#endif


// UP BUTTON ****************************************************
// single up button click
static void up_button_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_veryshort_pulse();
  if (currThrow.number == 19) {
    currThrow.number += 1;
  } else {
    currThrow.number += 5;
  }
  refresh_number();
}
// long up button press
static void up_button_long_press_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_veryshort_pulse();
  currThrow.number += 19;
  refresh_number();
}
static void up_button_long_release_handler(ClickRecognizerRef recognizer, void *context) {
}

// DOWN BUTTON ****************************************************
static void down_button_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_veryshort_pulse();
  currThrow.number += 1;
  refresh_number();
}
// long up button press
static void down_button_long_press_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_veryshort_pulse();
  currThrow.number += 16;
  refresh_number();
}
static void down_button_long_release_handler(ClickRecognizerRef recognizer, void *context) {
}

// SELECT BUTTON ****************************************************
static void select_button_timer_callback(void *data) {
  if (select_button_timer_count < 5) {
    select_button_timer_count++;
    select_state++;
    vibes_veryshort_number(select_state + 1);
    if (select_state < 4) {
      select_button_timer = app_timer_register(500, select_button_timer_callback, NULL);
    }
    switch(select_state) {
      case 1: x01_gui_draw_status("DOUBLE"); break;
      case 2: x01_gui_draw_status("TRIPLE"); break;
      case 3: x01_gui_draw_status("CANCEL"); break;
      case 4: x01_gui_draw_status("CANCEL ROUND"); break;
    }
  } else {
    app_timer_cancel(select_button_timer);
  }
}

static void select_button_down_handler(ClickRecognizerRef recognizer, void *context) {
  select_state = 0;
  select_button_timer_count = 0;
  select_button_timer = app_timer_register(500, select_button_timer_callback, NULL);
}

static void select_button_up_handler(ClickRecognizerRef recognizer, void *context) {
  int eraseNum;
  GamePlayer *currentPlayer;
  app_timer_cancel(select_button_timer);
  switch(select_state) {
    case 0:
      // OK pressed
      process_throw();
      break;
    case 1:
      currThrow.modifier = 2;
      refresh_number();
      break;
    case 2:
      currThrow.modifier = 3;
      refresh_number();
      break;
    case 3:
      currThrow.modifier = 1;
      currThrow.number = 0;
      refresh_number();
      break;
    case 4:
      currThrow.modifier = 1;
      currThrow.number = 0;
      currentPlayer = &game->players[game->currentPlayer];
      eraseNum = 0;
      for(int i = 0; i < 3; i++) {
        eraseNum += currentPlayer->throws[game->currentRound][i].number * currentPlayer->throws[game->currentRound][i].modifier;
        currentPlayer->throws[game->currentRound][i].number = 0;
        currentPlayer->throws[game->currentRound][i].modifier = 1;
      }
      currentPlayer->currentThrow = 0;
      currentPlayer->thrownSum -= eraseNum;
      x01_gui_draw_throw_clear_all();
      x01_gui_draw_player_score(game->currentPlayer, game->goalNumber - game->players[game->currentPlayer].thrownSum);

      refresh_number();
      break;
  }
}

// END BUTTON HANDLERS ****************************************************



static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_button_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 400, up_button_long_press_handler, up_button_long_release_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_button_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 400, down_button_long_press_handler, down_button_long_release_handler);
  window_raw_click_subscribe(BUTTON_ID_SELECT, select_button_down_handler, select_button_up_handler, NULL);
}

static void reset_game() {
  game->currentPlayer = 0;
  game->currentRound = 0;

  for(int i = 0; i < game->numOfPlayers; i++) {
    game->players[i].currentThrow = 0;
    game->players[i].thrownSum = 0;
    memset(game->players[i].throws, 0, NUM_OF_MAX_ROUNDS_IN_GAME * 3 * sizeof(OneThrow));

    x01_gui_draw_player_score(i, game->goalNumber);
  }
  x01_gui_draw_throw_clear(0);
  x01_gui_draw_game_name(game->goalNumber);
  x01_gui_draw_game_settings_2b(game->isDoubleIn, game->isDoubleOut);

  currThrow.modifier = 1;
}

static void draw_game() {
  x01_gui_player_on(game->currentPlayer);

  GamePlayer *currentPlayer = &game->players[game->currentPlayer];
  //draw the history
  int histCount = 0;
  for(int i = game->currentRound -1; i >= 0 && histCount < HISTORY_SIZE; i--) {
    int sum = 0;
    for(int x = 0; x < 3; x++) {
      sum += currentPlayer->throws[i][x].number * currentPlayer->throws[i][x].modifier;
    }
    x01_gui_draw_prev_round(histCount, sum);
    histCount++;
  }

  for(int i = 0; i < game->numOfPlayers; i++) {
    // update the score of the latest player
    x01_gui_draw_player_score(i, game->goalNumber - game->players[i].thrownSum);
  }
  x01_gui_draw_game_name(game->goalNumber);
  x01_gui_draw_game_round(game->currentRound + 1);
  x01_gui_draw_game_settings_2b(game->isDoubleIn, game->isDoubleOut);

  for(int i = 0; i < 3; i++) {
    if (currentPlayer->throws[game->currentRound][i].number > 0) {
      refresh_throw(i, currentPlayer->throws[game->currentRound][i].number, currentPlayer->throws[game->currentRound][i].modifier);
    }
  }

  refresh_number();
}

void hide_window_ui(void) {
  x01_gui_hide_window_ui();
}

void x01_window_push(Game *pgame, int newGame) {
  x01_gui_window_push(click_config_provider);
  if (newGame) {
    currThrow.number = 0;
    currThrow.modifier = 1;
    game = pgame;
    game->currentPlayer = 0;
    reset_game();
  } else {
    draw_game();
  }
}
