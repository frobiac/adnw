#include <avr/pgmspace.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include "active_keys.h"

void
ActiveKeys__reset(ActiveKeys *this)
{
  memset(&this->keys[0], UCHAR_MAX, MAX_ACTIVE_CELLS * sizeof(this->keys[0]));
  this->num_keys = 0;
}

bool
ActiveKeys__add_cell(ActiveKeys *this, Cell cell)
{
  if (this->num_keys > MAX_ACTIVE_CELLS)
    return false;

  BoundKey__init(&this->keys[this->num_keys++], cell);
  return true;
}

uint8_t
ActiveKeys__count(ActiveKeys *this)
{
  uint8_t count = 0;
  for (uint8_t i = 0; i < this->num_keys; ++i)
  {
    if (BoundKey__is_active(&this->keys[i]))
      ++count;
  }
  return count;
}

BoundKey*
ActiveKeys__first(ActiveKeys *this)
{
  this->curr_key = 0;
  if (BoundKey__is_active(&this->keys[this->curr_key]))
    return &this->keys[this->curr_key];
  return ActiveKeys__next(this);
}

BoundKey*
ActiveKeys__next(ActiveKeys *this)
{
  BoundKey *key = NULL;
  while (!key && this->curr_key < this->num_keys)
  {
    key = &this->keys[++this->curr_key];
    if (!BoundKey__is_active(key))
      key = NULL;
  }
  return key;
}

