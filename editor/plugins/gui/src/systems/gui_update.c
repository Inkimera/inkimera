#include "project.h"

#include "gui_node.h"
#include "widgets/gui_menu_header_widget.h"
#include "widgets/gui_scene_widget.h"
#include "widgets/gui_scene_graph_widget.h"

#include "systems/gui_update.h"

/* gui_plugin_build_render_graph */
static void
gui_plugin_build_render_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container,
  ecs_entity_t node
);

/* gui_plugin_build_header */
/*
   static void
   gui_plugin_build_header(
   struct nk_context *nk_ctx,
   int header_size
   ) {
   nk_style_push_color(nk_ctx, &nk_ctx->style.window.background, INK_BLACK_INK);
   nk_style_push_style_item(nk_ctx, &nk_ctx->style.window.fixed_background, nk_style_item_color(INK_BLACK_INK));
   if (nk_begin(nk_ctx, "InkMenuBar", nk_rect(0, 0, GetRenderWidth(), header_size), NK_WINDOW_NO_SCROLLBAR)) {
    static enum nk_collapse_states state;
    nk_layout_space_begin(nk_ctx, NK_STATIC, 64, 2);
    nk_menubar_begin(nk_ctx);
    nk_layout_space_push(nk_ctx, nk_rect(10, 16, 100, 32));
    if (nk_menu_begin_label(nk_ctx, "Project", NK_LEFT, (struct nk_vec2) { 100.0, 100.0 })) {
      nk_layout_row_static(nk_ctx, 0.0, 100.0, 1);
      nk_menu_item_label(nk_ctx, "New", NK_TEXT_LEFT);
      nk_menu_item_label(nk_ctx, "Open", NK_TEXT_LEFT);
      nk_menu_item_label(nk_ctx, "Save", NK_TEXT_LEFT);
      nk_menu_item_label(nk_ctx, "Exit", NK_TEXT_LEFT);
      nk_menu_end(nk_ctx);
    }
    nk_layout_space_push(nk_ctx, nk_rect(110, 16, 100, 32));
    if (nk_menu_begin_label(nk_ctx, "Scene", NK_LEFT, (struct nk_vec2) { 100.0, 100.0 })) {
      nk_layout_row_static(nk_ctx, 0.0, 100.0, 1);
      nk_menu_item_label(nk_ctx, "New", NK_TEXT_LEFT);
      nk_menu_item_label(nk_ctx, "Open", NK_TEXT_LEFT);
      nk_menu_item_label(nk_ctx, "Save", NK_TEXT_LEFT);
      nk_menu_end(nk_ctx);
    }
    nk_menubar_end(nk_ctx);
    nk_layout_space_push(nk_ctx, nk_rect(GetRenderWidth() / 2 - 100, 16, 200, 32));
    nk_style_push_color(nk_ctx, &nk_ctx->style.text.color, INK_WHITE);
    nk_label(nk_ctx, "Untitled*", NK_TEXT_LEFT);
    nk_style_pop_color(nk_ctx);
    nk_layout_space_end(nk_ctx);
   }
   nk_end(nk_ctx);
   nk_style_pop_color(nk_ctx);
   nk_style_pop_style_item(nk_ctx);
   }
 */

/* gui_plugin_build_footer */
static void
gui_plugin_build_footer(
  struct nk_context *nk_ctx,
  int footer_size
) {
}

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
    rect->x = container.x + container.width * position->value.dynamic.x;
    rect->y = container.y + container.height * position->value.dynamic.y;
  }
  if (size->type == GuiDimensionTypeFixed) {
    rect->width = size->value.fixed.width;
    rect->height = size->value.fixed.height;
  } else {
    rect->width = container.width * size->value.dynamic.width;
    rect->height = container.height * size->value.dynamic.height;
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
  ecs_entity_t project_node,
  const project_t *project,
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
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_a_rect, child_a);
    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_b_rect, child_b);
  } else {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_b_rect, child_a);
    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_a_rect, child_b);
  }
}

static void
gui_plugin_hsplit_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
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
        printf("Invalid number of children for GuiNodeHSplit\n");
        return;
      }
      children[count++] = it.entities[i];
    }
  }
  const gui_anchor_t *child_anchor = ecs_get(ecs_ctx, children[0], gui_anchor_t);
  if (child_anchor && *child_anchor == GUI_ANCHOR_A) {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_a_rect, child_a);
    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_b_rect, child_b);
  } else {
    ecs_entity_t child_a = children[0];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_b_rect, child_a);
    ecs_entity_t child_b = children[1];
    gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, child_a_rect, child_b);
  }
}

/* gui_plugin_build_render_graph */
static void
gui_plugin_build_render_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
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
    int padding = 0;
    Rectangle rect, inner_rect;
    rect_with_bounds(&rect, &inner_rect, container, position, size, padding);
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, inner_rect, child);
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
        gui_plugin_build_render_graph(it.world, nk_ctx, project_node, project, rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_VSPLIT) {
    gui_plugin_vsplit_graph(ecs_ctx, nk_ctx, project_node, project, container, node);
  } else if (gui_type == GUI_NODE_HSPLIT) {
    gui_plugin_hsplit_graph(ecs_ctx, nk_ctx, project_node, project, container, node);
  } else if (gui_type == GUI_NODE_WIDGET) {
    const gui_state_t *state = ecs_get(ecs_ctx, node, gui_state_t);
    const gui_widget_type_t *widget_type = ecs_get(ecs_ctx, node, gui_widget_type_t);
    if (!state || !widget_type) {
      printf("Invalid GuiNodeWidget\n");
      return;
    }
    float width = 200;
    float height = 30;
    Rectangle rect = {
      .x = container.x + container.width - width,
      .y = container.y,
      .width = width,
      .height = height
    };
    switch (*widget_type) {
    case GUI_WIDGET_SCENE: {
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
          if (state->target == 0 || gui_hash(scene_data->name) == state->target) {
            gui_scene_widget(it.world, nk_ctx, project_node, project, container, node, scene);
            break;
          }
        }
      }
      ecs_filter_fini(f);
      break;
    }
    case GUI_WIDGET_SCENE_GRAPH: {
      gui_scene_graph_widget(ecs_ctx, nk_ctx, container, node);
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
  const gui_texture_t *gui_texture = ecs_singleton_get(it->world, gui_texture_t);
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  for (int i = 0; i < it->count; i++) {
    ecs_entity_t window = it->entities[i];
    ecs_entity_t project_node = ecs_get_target(it->world, window, ProjectOf, 0);
    const project_t *project = ecs_get(it->world, project_node, project_t);
    int header_size = 64.0;
    gui_header_widget(it->world, nk_ctx, project_node, project, (Rectangle) { 0, 0, GetRenderWidth(), header_size });
    gui_plugin_build_render_graph(it->world, nk_ctx, project_node, project, (Rectangle) { 0, header_size, GetRenderWidth(), GetRenderHeight() - header_size }, window);
    nk_style_push_style_item(nk_ctx, &nk_ctx->style.window.fixed_background, nk_style_item_nine_slice(nk_sub9slice_id(gui_texture->texture.id, 2048, 2048, nk_rect(0, 1024, 1024, 1024), 4, 2, 6, 8)));
    if (nk_begin(nk_ctx, "InkBackground", nk_rect(0, 0, GetRenderWidth(), GetRenderHeight()), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND | NK_WINDOW_NO_INPUT)) {
    }
    nk_end(nk_ctx);
    nk_style_pop_style_item(nk_ctx);
  }
}
