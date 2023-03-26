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

/* gui_texture_t */
ECS_COMPONENT_DECLARE(gui_texture_t);

/* gui_texture_on_remove */
void
gui_texture_on_remove(
  ecs_iter_t *it
) {
  ecs_world_t *ecs_ctx = it->world;
  ecs_entity_t event = it->event;
  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    const gui_texture_t *texture = ecs_get(ecs_ctx, e, gui_texture_t);
    printf("gui_texture_on_remove(%p)\n", (void*)texture);
    UnloadTexture(texture->texture);
  }
}
/* gui_components_init */
int
gui_components_init(
  engine_t *eng
) {
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, script_t);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiRoot);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiFocus);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_nk_context_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_texture_t);
  ecs_set_hooks(eng->ecs_ctx, gui_texture_t, {
    //.ctor = ecs_ctor(threed_render_texture_t),
    //.dtor = ecs_dtor(threed_render_texture_t),
    //.move = ecs_move(threed_render_texture_t),
    //.copy = ecs_copy(threed_render_texture_t)
    .on_remove = gui_texture_on_remove
  });
  ecs_singleton_set(eng->ecs_ctx, gui_nk_context_t, { .nk_ctx = eng->nk_ctx });
  return 0;
}
