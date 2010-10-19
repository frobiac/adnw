#ifndef __ACTIVE_KEYS_H__
#define __ACTIVE_KEYS_H__

#include "bound_key.h"

#define NUM_MODIFIERS 8
#define MAX_KEYS      6
#define MAX_ACTIVE_CELLS (MAX_KEYS + NUM_MODIFIERS)

struct ActiveKeys
{
  BoundKey  keys[MAX_ACTIVE_CELLS];
  uint8_t   num_keys;
  uint8_t   curr_key;
};

typedef struct ActiveKeys ActiveKeys;

void      ActiveKeys__reset(ActiveKeys *this);
bool      ActiveKeys__add_cell(ActiveKeys *this, Cell cell);

uint8_t   ActiveKeys__count(ActiveKeys *this);
BoundKey* ActiveKeys__first(ActiveKeys *this);
BoundKey* ActiveKeys__next(ActiveKeys *this);


#endif // __ACTIVE_KEYS_H__
