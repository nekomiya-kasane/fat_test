#ifndef __GAME__
#define __GAME__

/** @brief Game states */
typedef enum {
  STATE_IDLE = 0,
  STATE_GAMING = 1,
  STATE_GAMEOVER = 2,
  STATE_WON = 3
} State;

typedef void* GameHandle;

/** @brief Initialize a game context */
GameHandle init_game(int row, int col, int fps);

/** @brief Process user input */
int process_input(GameHandle handle);

/** @brief Run the game loop */
State run(GameHandle handle);

/** @brief Destroy the game context */
int destroy_game(GameHandle handle);

#endif