#ifndef ENGINE_H
#define ENGINE_H
#include "inkimera.h"
#include "signal.h"

/*
 * ENGINE
 */

/* engine_t */
struct engine {
  Font font;
  struct nk_context *nk_ctx;
  ecs_world_t *ecs_ctx;
};
#endif
