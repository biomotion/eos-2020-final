#ifndef __PKT_UTIL_H__
#define __PKT_UTIL_H__

#include "mole.h"
#define NUM_MOLES 9
#define NUM_PLAYERS 2

typedef struct
{
  int gameState;
  int score;
  struct Mole mole_states[NUM_MOLES];
} server_pkt;

#endif