#include <pebble.h>

#define NUM_OF_MAX_ROUNDS_IN_GAME 30
#define MAX_NUM_OF_PLAYERS 4
#define HISTORY_SIZE 4


void x01_gui_window_push(ClickConfigProvider click_config_provider);
void x01_gui_destroy_ui(void);
void x01_gui_hide_window_ui(void);

/**
 * Draw the score of a previous round. 0 previous, 1 before the previous
 * round: 0..3
 * value: 0..60
 */
void x01_gui_draw_prev_round(int round, int value);

/**
 * Draw the score of the current round
 * This should be updated after each throw
 * value: 0..60
 */
void x01_gui_draw_curr_round(int value);
void x01_gui_draw_curr_round_clear();

/**
 * Draw the throw
 * count: 0..2 number of the throw in the current round
 * value: d5, t12, dBU, 1, 4, 19, 20... the throw string
 */
void x01_gui_draw_throw(int count, char* value);
void x01_gui_draw_throw_clear(int count);
void x01_gui_draw_throw_clear_all();

/**
 * Draw the score of the selected player
 * player: 0..4
 * score: 301..0 (depends on the game)
 */
void x01_gui_draw_player_score(int player, int score);

/** Show status (double selected, or any text) */
void x01_gui_draw_status(char* s);

/** Draw the number of the current round*/
void x01_gui_draw_game_round(int number);

/** Draw the name of the game */
void x01_gui_draw_game_name(int number);

/** Changes the player's background color.
 * on: the player playing
 * off: not playing right now
 */
void x01_gui_player_on(int i);
void x01_gui_player_off(int i);

/**
 * Draw the game settings below the first line
 * s: "Double in: No  Double out: Yes"
 */
void x01_gui_draw_game_settings(char* s);
void x01_gui_draw_game_settings_2b(bool isDoubleIn, bool isDoubleOut);

