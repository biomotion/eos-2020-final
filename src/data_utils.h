#ifndef __PKT_UTIL_H__
#define __PKT_UTIL_H__

#define NUM_MOLES 9
#define NUM_PLAYERS 2

#define GAME_STATE_INIT 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_END 2

typedef struct
{
  int gameState;
  int score;
  char mole_states[NUM_MOLES];
} server_pkt;

#endif