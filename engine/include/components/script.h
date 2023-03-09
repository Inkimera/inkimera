#ifndef INKIMERA_SCRIPT_H
#define INKIMERA_SCRIPT_H

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

/*
 * CORE
 */

/* script_state_t */
typedef struct {
  lua_State *state;
} script_state_t;

/* script_t */
typedef struct {
  char *src;
} script_t;
#endif
