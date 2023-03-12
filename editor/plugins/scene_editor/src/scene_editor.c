#include <stdlib.h>

#include "scene_editor.h"

/* SceneEditorPlugin */
ECS_TAG_DECLARE(SceneEditorPlugin);

/* scene_editor_plugin_init */
node_id_t
scene_editor_plugin_init(
  engine_t *eng
) {
  printf("INIT scene_editor plugin\n");
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
  ECS_TAG_DEFINE(ecs_ctx, SceneEditorPlugin);
  ECS_SYSTEM(ecs_ctx, scene_editor_system, InkOnUpdate, plugin_t, SceneEditorPlugin);
  ECS_SYSTEM(ecs_ctx, scene_editor_scene_system, InkOnUpdate, threed_camera_t, threed_position_t, threed_rotation_t);

  ecs_entity_t plugin = ecs_new_id(ecs_ctx);
  scene_editor_plugin_t *plug = malloc(sizeof(scene_editor_plugin_t));
  ecs_add(ecs_ctx, plugin, SceneEditorPlugin);
  ecs_set(ecs_ctx, plugin, plugin_t, {
    .eng = eng,
    .plugin = plug,
    .load = &scene_editor_plugin_load,
    .unload = &scene_editor_plugin_unload
  });
  return plugin;
}

/* scene_editor_plugin_deinit */
int
scene_editor_plugin_deinit(
  scene_editor_plugin_t *plug
) {
  printf("DEINIT scene_editor plugin\n");
  free(plug);
  return 0;
}

/* scene_editor_plugin_load */
int
scene_editor_plugin_load(
  engine_t *eng,
  void *plug
) {
  printf("LOAD scene_editor plugin\n");
  return 0;
}

/* scene_editor_plugin_unload */
int
scene_editor_plugin_unload(
  engine_t *eng,
  void *plug
) {
  printf("UNLOAD scene_editor plugin\n");
  (void)eng;
  scene_editor_plugin_t *scene_editor_plug = (scene_editor_plugin_t*)plug;
  return scene_editor_plugin_deinit(scene_editor_plug);
}

/* scene_editor_system */
void
scene_editor_system(
  ecs_iter_t *it
) {
  const plugin_t *plugs = ecs_field(it, plugin_t, 1);
  engine_t *eng = plugs[0].eng;
  scene_editor_plugin_t *scene_editor_plug = (scene_editor_plugin_t*)plugs[0].plugin;
}

/* scene_editor_scene_system */
void
scene_editor_scene_system(
  ecs_iter_t *it
) {
  const threed_camera_t *cameras = ecs_field(it, threed_camera_t, 1);
  const threed_position_t *positions = ecs_field(it, threed_position_t, 2);
  const threed_rotation_t *rotations = ecs_field(it, threed_rotation_t, 3);
}
