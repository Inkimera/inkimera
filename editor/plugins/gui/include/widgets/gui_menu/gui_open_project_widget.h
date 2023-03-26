#ifndef GUI_OPEN_PROJECT_WIDGET_H
#define GUI_OPEN_PROJECT_WIDGET_H

#include "inkimera.h"

#include "project.h"

/* gui_open_project_widget */
bool
gui_open_project_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container,
  bool *show_file_browser
);
#endif
