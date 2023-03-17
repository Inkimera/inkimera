#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"

#include "engine.h"
#include "components/gui_components.h"

/* script_state_t */
ECS_COMPONENT_DECLARE(script_state_t);

/* script_t */
ECS_COMPONENT_DECLARE(script_t);

/*
 * GUI CORE
 */

/* GuiRoot */
ECS_TAG_DECLARE(GuiRoot);

/* GuiFocus */
ECS_TAG_DECLARE(GuiFocus);

/* gui_nk_context_t */
ECS_COMPONENT_DECLARE(gui_nk_context_t);

/* gui_components_init */
int
gui_components_init(
  engine_t *eng
) {
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, script_t);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiRoot);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiFocus);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_nk_context_t);
  ecs_singleton_set(eng->ecs_ctx, gui_nk_context_t, { .nk_ctx = eng->nk_ctx });
  return 0;
}
