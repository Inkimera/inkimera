#include <stdlib.h>

#include "gui_node.h"
#include "widgets/gui_scene_graph_widget.h"

static const float ratio[] = { 120, 150 };

/* gui_scene_graph_widget_nodes */
static void
gui_scene_graph_widget_nodes(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  const gui_state_t *state,
  Rectangle container,
  ecs_entity_t node
) {
  char tree_name[128] = { 0 };
  int len = snprintf(tree_name, 128, "3dNode(%lld)", (long long)node);
  static ecs_entity_t selected_idx = -1;
  nk_bool selected = false;
  if (selected_idx == node) {
    selected = true;
  }
  ecs_iter_t it = ecs_children(ecs_ctx, node);
  if (it.count == 0) {
    if (nk_selectable_label(nk_ctx, tree_name, NK_TEXT_LEFT, &selected)) {
      selected_idx = node;
    }
  } else {
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
      while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
          ecs_entity_t child = it.entities[i];
          gui_scene_graph_widget_nodes(ecs_ctx, nk_ctx, state, container, child);
        }
      }
      nk_tree_element_pop(nk_ctx);
    }
  }
}

/* gui_scene_graph_widget */
void
gui_scene_graph_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t widget
) {
  char window_name[128] = { 0 };
  //static bool show_add_menu = false;
  const gui_state_t *state = ecs_get(ecs_ctx, widget, gui_state_t);
  if (!state) {
    ecs_set(ecs_ctx, widget, gui_state_t, { .mode = 0, .target = 0 });
    state = ecs_get(ecs_ctx, widget, gui_state_t);
  }
  int len = snprintf(window_name, 128, "GUI_WIDGET_SCENE_GRAPH(%lld)", (long long)widget);
  if (nk_begin(nk_ctx, window_name, nk_rect(container.x, container.y, container.width, container.height), NK_WINDOW_SCROLL_AUTO_HIDE)) {
    static char filter[128] = { 0 };
    static int filter_len = 0;
    nk_layout_row_begin(nk_ctx, NK_DYNAMIC, 50, 3);
    nk_layout_row_push(nk_ctx, 0.20);
    nk_label(nk_ctx, "Filter:", NK_TEXT_CENTERED);
    nk_layout_row_push(nk_ctx, 0.60);
    nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, filter, &filter_len, 64, nk_filter_default);
    nk_layout_row_push(nk_ctx, 0.20);
    if (nk_button_label(nk_ctx, "Add")) {
      //show_add_menu = true;
    }
    //if (show_add_menu) {
    //  int popup_width = 400;
    //  int popup_height = 600;
    //  nk_popup_begin(nk_ctx, NK_POPUP_STATIC, "Add", NK_WINDOW_SCROLL_AUTO_HIDE, nk_rect(container.width / 2 - popup_width / 2, container.height / 2 - popup_height / 2, popup_width, popup_height));
    //  nk_popup_end(nk_ctx);
    //}
    nk_layout_row_end(nk_ctx);

    ecs_filter_t *f = ecs_filter(ecs_ctx, {
      .terms = {
        { .id = ecs_id(ThreeDScene), .inout = EcsIn },
      }
      });
    ecs_iter_t it = ecs_filter_iter(ecs_ctx, f);
    while (ecs_iter_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        node_id_t scene = it.entities[i];
        const threed_scene_t *scene_data = ecs_get(ecs_ctx, scene, threed_scene_t);
        if (nk_tree_push_hashed(nk_ctx, NK_TREE_NODE, scene_data->name, NK_MINIMIZED, scene_data->name, len, 0)) {
          ecs_iter_t it = ecs_children(ecs_ctx, scene);
          while (ecs_children_next(&it)) {
            for (int i = 0; i < it.count; i++) {
              ecs_entity_t child = it.entities[i];
              gui_scene_graph_widget_nodes(ecs_ctx, nk_ctx, state, container, child);
            }
          }
          nk_tree_pop(nk_ctx);
        }
      }
    }
    ecs_filter_fini(f);
  }
  nk_end(nk_ctx);
}
