#ifndef GUI_MENU_HEADER_WIDGET_H
#define GUI_MENU_HEADER_WIDGET_H

#include "inkimera.h"

#include "project.h"

/* gui_header_widget */
void
gui_header_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container
);
#endif
