#include "stdlib.h"
#include "flecs.h"

#include "engine.h"
#include "components/plugin_components.h"

/* plugin_t */
ECS_COMPONENT_DECLARE(plugin_t);

ECS_CTOR(plugin_t, plug, {
  printf("plugin_t load\n");
  plug->load = NULL;
  plug->unload = NULL;
})

ECS_DTOR(plugin_t, plug, {
  printf("plugin_t unload\n");
  plug->unload(plug->eng, plug->plugin);
})

void
plugin_on_set(
  ecs_iter_t *it
) {
  ecs_world_t *ecs_ctx = it->world;
  ecs_entity_t event = it->event;
  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    const plugin_t *plug = ecs_get(ecs_ctx, e, plugin_t);
    printf("plugin_on_set load_handle_t(%p) engine_t(%p) plugin_t(%p)\n", *plug->load, plug->eng, plug->plugin);
    plug->load(plug->eng, plug->plugin);
    //ecs_trace("%s: %s", ecs_get_name(world, event), ecs_get_name(world, e));
  }
}

/* plugin_components_init */
int
plugin_components_init(
  engine_t *eng
) {
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, plugin_t);
  ecs_set_hooks(eng->ecs_ctx, plugin_t, {
    .ctor = ecs_ctor(plugin_t),
    .dtor = ecs_dtor(plugin_t),
    .on_set = plugin_on_set
  });
  return 0;
}
