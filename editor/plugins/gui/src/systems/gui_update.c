#include "gui_node.h"
#include "systems/gui_update.h"
#include "widgets/gui_scene_widget.h"
#include "widgets/gui_scene_graph_widget.h"

/* gui_plugin_build_render_graph */
static void
gui_plugin_build_render_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t node
);

/* rect_with_bounds */
static void
rect_with_bounds(
  Rectangle *rect,
  Rectangle *inner_rect,
  Rectangle container,
  const gui_position_t *position,
  const gui_size_t *size,
  int padding
) {
  if (position->type == GuiDimensionTypeFixed) {
    rect->x = position->value.fixed.x;
    rect->y = position->value.fixed.y;
  } else {
    rect->x =
      (int)((float)GetRenderWidth() * position->value.dynamic.x);
    rect->y =
      (int)((float)GetRenderHeight() * position->value.dynamic.y);
  }
  if (size->type == GuiDimensionTypeFixed) {
    rect->width = size->value.fixed.width;
    rect->height = size->value.fixed.height;
  } else {
    rect->width = (int)((float)GetRenderWidth() * size->value.dynamic.width);
    rect->height = (int)((float)GetRenderHeight() * size->value.dynamic.height);
  }
  *inner_rect = *rect;
  inner_rect->x += padding;
  inner_rect->y += padding;
  inner_rect->width -= padding * 2;
  inner_rect->height -= padding * 2;
}

static void
gui_plugin_vsplit_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t node
) {
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
  const gui_size_t *size = ecs_get(ecs_ctx, node, gui_size_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
  if (!anchor | !size || !layout) {
    printf("Invalid GuiNodeVSplit\n");
    return;
  }
  Rectangle child_a_rect = container;
  child_a_rect.width = (int)((float)container.width * size->value.dynamic.width);

  Rectangle child_b_rect = container;
  child_b_rect.width = (int)((float)container.width * (1.0 - size->value.dynamic.width));
  child_b_rect.x += child_a_rect.width;

  int count = 0;
  ecs_entity_t children[2];
  ecs_iter_t it = ecs_children(ecs_ctx, node);
  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      if (count > 2) {
        printf("Invalid number of children for GuiNodeVSplit\n");
        return;
      }
      children[count++] = it.entities[i];
    }
  }
  const gui_anchor_t *child_anchor = ecs_get(ecs_ctx, children[0], gui_anchor_t);
  if (child_anchor && *child_anchor == GUI_ANCHOR_A) {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_a_rect, child_a);

    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_b_rect, child_b);
  } else {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_b_rect, child_a);

    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_a_rect, child_b);
  }
}

static void
gui_plugin_hsplit_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t node
) {
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
  const gui_size_t *size = ecs_get(ecs_ctx, node, gui_size_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
  if (!anchor | !size || !layout) {
    printf("Invalid GuiNodeHSplit\n");
    return;
  }
  Rectangle child_a_rect = container;
  child_a_rect.height = (int)((float)container.height * size->value.dynamic.height);
  Rectangle child_b_rect = container;
  child_b_rect.height = (int)((float)container.height * (1.0 - size->value.dynamic.height));
  child_b_rect.y += child_a_rect.height;

  int count = 0;
  ecs_entity_t children[2];
  ecs_iter_t it = ecs_children(ecs_ctx, node);
  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      if (count > 2) {
        printf("Invalid number of children for GuiNodeVSplit\n");
        return;
      }
      children[count++] = it.entities[i];
    }
  }
  const gui_anchor_t *child_anchor = ecs_get(ecs_ctx, children[0], gui_anchor_t);
  if (child_anchor && *child_anchor == GUI_ANCHOR_A) {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_a_rect, child_a);

    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_b_rect, child_b);
  } else {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_b_rect, child_a);

    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, child_a_rect, child_b);
  }
}

/* gui_plugin_build_render_graph */
static void
gui_plugin_build_render_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t node
) {
  const gui_node_type_t *type = ecs_get(ecs_ctx, node, gui_node_type_t);
  if (!type) {
    printf("Invalid GuiNode\n");
    return;
  }
  gui_node_type_t gui_type = *type;
  if (gui_type == GUI_NODE_WINDOW) {
    const gui_label_t *label = ecs_get(ecs_ctx, node, gui_label_t);
    const gui_position_t *position = ecs_get(ecs_ctx, node, gui_position_t);
    const gui_size_t *size = ecs_get(ecs_ctx, node, gui_size_t);
    if (!label || !position || !size) {
      printf("Invalid GuiNodeWindow\n");
      return;
    }
    int padding = 8;
    Rectangle rect, inner_rect;
    rect_with_bounds(&rect, &inner_rect, container, position, size, padding);
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_plugin_build_render_graph(it.world, nk_ctx, inner_rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_PANE) {
    const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
    const gui_label_t *label = ecs_get(ecs_ctx, node, gui_label_t);
    const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
    if (!anchor | !label || !layout) {
      printf("Invalid GuiNodePane\n");
      return;
    }
    Rectangle rect = container;
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_plugin_build_render_graph(it.world, nk_ctx, rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_VSPLIT) {
    gui_plugin_vsplit_graph(ecs_ctx, nk_ctx, container, node);
  } else if (gui_type == GUI_NODE_HSPLIT) {
    gui_plugin_hsplit_graph(ecs_ctx, nk_ctx, container, node);
  } else if (gui_type == GUI_NODE_WIDGET) {
    const gui_state_t *state = ecs_get(ecs_ctx, node, gui_state_t);
    const gui_widget_type_t *widget_type = ecs_get(ecs_ctx, node, gui_widget_type_t);
    if (!state || !widget_type) {
      printf("Invalid GuiNodeWidget\n");
      return;
    }
    static const char *items[] = { "SCENE", "TERM", "PROPS", "NODES" };
    float width = 200;
    float height = 30;
    int mode = state->mode;
    Rectangle rect = {
      .x = container.x + container.width - width,
      .y = container.y,
      .width = width,
      .height = height
    };
    //char window_name[64] = { 0 };
    //sprintf(window_name, "DROPDOWN(%lld)", (long long)node);
    //if (nk_begin(nk_ctx, window_name, nk_rect(rect.x, rect.y, rect.width, rect.height), NK_WINDOW_NO_SCROLLBAR)) {
    //  nk_layout_row_static(nk_ctx, rect.height, rect.width, 1);
    //  mode = nk_combo(nk_ctx, items, 4, state->mode, rect.height, nk_vec2(rect.width, 200));
    //  if (mode != state->mode) {
    //    gui_state_t new_state = { .mode = mode };
    //    ecs_set_ptr(ecs_ctx, node, gui_state_t, &new_state);
    //    ecs_set(ecs_ctx, node, gui_widget_type_t, { mode });
    //  }
    //}
    //nk_end(nk_ctx);
    switch (*widget_type) {
    case GUI_WIDGET_SCENE: {
      ecs_filter_t *f = ecs_filter(ecs_ctx, {
          .terms = {
            { .id = ecs_id(ThreeDScene), .inout = EcsIn },
          }
      });
      ecs_iter_t it = ecs_filter_iter(ecs_ctx, f);
      node_id_t scene = ecs_iter_first(&it);
      if (scene) {
        gui_scene_widget(it.world, nk_ctx, container, node, scene);
      }
      ecs_filter_fini(f);
      break;
    }
    case GUI_WIDGET_SCENE_GRAPH: {
      ecs_filter_t *f = ecs_filter(ecs_ctx, {
          .terms = {
            { .id = ecs_id(ThreeDScene), .inout = EcsIn },
          }
      });
      ecs_iter_t it = ecs_filter_iter(ecs_ctx, f);
      node_id_t scene = ecs_iter_first(&it);
      if (scene) {
        gui_scene_graph_widget(it.world, nk_ctx, container, node, scene);
      }
      ecs_filter_fini(f);
      break;
    }
    default:
      break;
    }
    //ecs_iter_t it = ecs_children(ecs_ctx, node);
    //while (ecs_children_next(&it)) {
    //  for (int i = 0; i < it.count; i++) {
    //    ecs_entity_t child = it.entities[i];
    //    const gui_widget_type_t *widget_type = ecs_get(ecs_ctx, child, gui_widget_type_t);
    //    if (!widget_type) {
    //      printf("Invalid GuiWidget\n");
    //      return;
    //    }
    //  }
    //}
  } else {
  }
}
/* gui_plugin_update_system */
void
gui_plugin_update_system(
  ecs_iter_t *it
) {
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  for (int i = 0; i < it->count; i++) {
    gui_plugin_build_render_graph(it->world, nk_ctx, (Rectangle) { 0, 0, GetRenderWidth(), GetRenderHeight() }, it->entities[i]);
  }
}
