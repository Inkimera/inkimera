#ifndef GUI_SCENE_WIDGET_H
#define GUI_SCENE_WIDGET_H

#include "inkimera.h"

void
gui_scene_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t widget,
  ecs_entity_t scene
);
#endif
