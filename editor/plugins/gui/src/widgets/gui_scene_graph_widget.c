#include <stdlib.h>

#include "widgets/gui_scene_graph_widget.h"

static const float ratio[] = { 120, 150 };

/* gui_scene_graph_widget_nodes */
static void
gui_scene_graph_widget_nodes(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t node
) {
  char tree_name[128] = { 0 };
  int len = sprintf(tree_name, "3dNode(%lld)", (long long)node);
  static nk_bool selected = 0;
  if (nk_tree_element_push_hashed(nk_ctx, NK_TREE_NODE, tree_name, NK_MINIMIZED, &selected, tree_name, len, 0)) {
    /*
       const threed_position_t *pos = ecs_get(ecs_ctx, node, threed_position_t);
       char x[32] = { 0 };
       int x_len = snprintf(x, 32, "%f", pos->x);
       char y[32] = { 0 };
       int y_len = snprintf(y, 32, "%f", pos->y);
       char z[32] = { 0 };
       int z_len = snprintf(z, 32, "%f", pos->z);

       nk_layout_row(nk_ctx, NK_STATIC, 50, 2, ratio);
       nk_label(nk_ctx, "Position X: ", NK_TEXT_LEFT);
       nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, x, &x_len, 64, nk_filter_float);
       nk_label(nk_ctx, "Position Y: ", NK_TEXT_LEFT);
       nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, y, &y_len, 64, nk_filter_float);
       nk_label(nk_ctx, "Position Z: ", NK_TEXT_LEFT);
       nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, z, &z_len, 64, nk_filter_float);
       threed_position_t new_pos = {
       .x = atof(x),
       .y = atof(y),
       .z = atof(z)
       };
       if (new_pos.x != pos->x || new_pos.y != pos->y || new_pos.z != pos->z) {
       ecs_set_ptr(ecs_ctx, node, threed_position_t, &new_pos);
       }
     */
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_scene_graph_widget_nodes(ecs_ctx, nk_ctx, container, child);
      }
    }
    nk_tree_element_pop(nk_ctx);
  }
}

/* gui_scene_graph_widget */
void
gui_scene_graph_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t widget,
  ecs_entity_t scene
) {
  char window_name[128] = { 0 };
  int len = sprintf(window_name, "GUI_WIDGET_SCENE_GRAPH(%lld)", (long long)widget);
  if (nk_begin(nk_ctx, window_name, nk_rect(container.x, container.y, container.width, container.height), NK_WINDOW_SCROLL_AUTO_HIDE | NK_WINDOW_BACKGROUND)) {
    if (nk_tree_push_hashed(nk_ctx, NK_TREE_NODE, window_name, NK_MINIMIZED, window_name, len, 0)) {
      ecs_iter_t it = ecs_children(ecs_ctx, scene);
      while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
          ecs_entity_t child = it.entities[i];
          gui_scene_graph_widget_nodes(ecs_ctx, nk_ctx, container, child);
        }
      }
      nk_tree_pop(nk_ctx);
    }
  }
  nk_end(nk_ctx);
}
