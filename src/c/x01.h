#pragma once

#include "x01_gui.h"
typedef struct {
  uint8_t number; // 0: missed, 1..20, 25: bull
  uint8_t modifier; // 1: simple, 2: double, 3: tripple

} OneThrow;

typedef struct {
  char initial;    // initial of the player
  bool isMe;    // is this player the owner of the watch? (plan to use it for online score keeping)
  uint8_t currentThrow; // starts at 0, number of throws (0..2)
  uint16_t thrownSum;
  OneThrow throws[NUM_OF_MAX_ROUNDS_IN_GAME][3];
} GamePlayer;

typedef struct {
  uint8_t numOfPlayers;
  uint8_t currentPlayer; // starts at 0
  uint8_t currentRound; // starts at 0
  GamePlayer players[MAX_NUM_OF_PLAYERS];
  uint16_t goalNumber; // 301, 501...
  bool isDoubleIn;
  bool isDoubleOut;
  uint8_t winner; // 1..4 the winner of the game, or 0
} Game;


void x01_window_push(Game *pgame, int newGame);

void show_window_ui(void);
void hide_window_ui(void);
