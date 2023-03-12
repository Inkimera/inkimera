#ifndef SCENE_EDITOR_H
#define SCENE_EDITOR_H

#include "inkimera.h"

/* SceneEditorPlugin */
extern ECS_TAG_DECLARE(SceneEditorPlugin);

/* scene_editor_plugin_t */
typedef struct {
  node_id_t scene;
} scene_editor_plugin_t;

/* scene_editor_plugin_init */
node_id_t
scene_editor_plugin_init(
  engine_t *eng
);

/* scene_editor_plugin_deinit */
int
scene_editor_plugin_deinit(
  scene_editor_plugin_t *plug
);

/* scene_editor_plugin_load */
int
scene_editor_plugin_load(
  engine_t *eng,
  void *plug
);

/* scene_editor_plugin_unload */
int
scene_editor_plugin_unload(
  engine_t *eng,
  void *plug
);

/* scene_editor_system */
void
scene_editor_system(
  ecs_iter_t *it
);

/* scene_editor_scene_system */
void
scene_editor_scene_system(
  ecs_iter_t *it
);
#endif
