#define NUM_OF_MAX_ROUNDS_IN_GAME 30
#define MAX_NUM_OF_PLAYERS 4
#define HISTORY_SIZE 4

typedef struct {
  uint8_t number; // 0: missed, 1..20, 25: bull
  uint8_t modifier; // 1: simple, 2: double, 3: tripple

} OneThrow;

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
  GamePlayer players[MAX_NUM_OF_PLAYERS];
  uint16_t goalNumber; // 301, 501...
  bool isDoubleIn;
  bool isDoubleOut;
} Game;


void x01_window_push(Game *pgame);

void show_window_ui(void);
void hide_window_ui(void);
