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
ECS_COMPONENT_DECLARE(script_state_t);

/* script_t */
typedef struct {
  char *src;
} script_t;
ECS_COMPONENT_DECLARE(script_t);
