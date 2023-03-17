#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"

#include "engine.h"
#include "pipeline.h"
#include "components/gui_components.h"
#include "systems/gui_systems.h"
#include "components/3d_components.h"

/* gui_system_init */
int
gui_systems_init(
  engine_t *eng
) {
  ECS_SYSTEM(eng->ecs_ctx, gui_node_input_system, InkOnInput, GuiRoot);
  ECS_SYSTEM(eng->ecs_ctx, gui_node_render_system, InkOnGuiRender, GuiRoot);
  return 0;
}

/*
 * INPUT
 */
/* gui_node_input_system */
void
gui_node_input_system(
  ecs_iter_t *it
) {
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  nk_raylib_input(nk_ctx);
}

/*
 * RENDER
 */
/* gui_node_render_system */
void
gui_node_render_system(
  ecs_iter_t *it
) {
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  nk_raylib_draw(nk_ctx);
}
