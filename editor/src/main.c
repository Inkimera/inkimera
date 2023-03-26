#include "stdlib.h"

#include "inkimera.h"

#include "project.h"
#include "gui.h"
#include "scene_editor.h"


/* register_lib_components */
void
register_lib_components(
  engine_t *eng
) {
  printf("INIT editor\n");
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  ECS_ENTITY_DEFINE(ecs_ctx, ProjectOf);
  ECS_ENTITY_DEFINE(ecs_ctx, ProjectFor);
  ECS_COMPONENT_DEFINE(ecs_ctx, project_scene_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, project_t);
}

/*
 * MAIN
 */
int
main(
  void
) {
  // INIT
  engine_t *eng = ink_init();
  register_lib_components(eng);
  // Plugins
  gui_plugin_init(eng);
  scene_editor_plugin_init(eng);
  // RUN
  ink_run(eng);
  // DEINIT
  ink_deinit(eng);
  return 0;
}
