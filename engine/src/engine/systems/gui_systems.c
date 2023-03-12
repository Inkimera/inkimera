#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"

#include "engine.h"
#include "pipeline.h"
#include "components/gui_components.h"
#include "systems/gui_systems.h"
#include "components/3d_components.h"

/* gui_system_init */
int
gui_systems_init(
  engine_t *eng
) {
  ECS_SYSTEM(eng->ecs_ctx, gui_node_input_system, InkOnInput, GuiRoot);
  ECS_SYSTEM(eng->ecs_ctx, gui_node_update_system, InkOnUpdate, GuiRoot);
  ECS_SYSTEM(eng->ecs_ctx, gui_node_render_system, InkOnRender, GuiRoot);
  return 0;
}

/*
 * RENDER
 */

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

/* gui_node_render_system */
void
gui_node_input_system(
  ecs_iter_t *it
) {
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  nk_raylib_input_mouse(nk_ctx);
}

/* gui_build_render_graph */
void
gui_build_render_graph(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t node
) {
  const gui_node_type_t *type = ecs_get(ecs_ctx, node, gui_node_type_t);
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
  const gui_label_t *label = ecs_get(ecs_ctx, node, gui_label_t);
  const gui_list_t *list = ecs_get(ecs_ctx, node, gui_list_t);
  const gui_position_t *position = ecs_get(ecs_ctx, node, gui_position_t);
  const gui_size_t *size = ecs_get(ecs_ctx, node, gui_size_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
  const gui_state_t *state = ecs_get(ecs_ctx, node, gui_state_t);
  if (!type) {
    printf("Invalid GuiNode\n");
    return;
  }
  gui_node_type_t gui_type = *type;
  if (gui_type == GUI_NODE_WINDOW) {
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
        gui_build_render_graph(ecs_ctx, nk_ctx, inner_rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_CONTAINER) {
    Rectangle rect = container;
    if (nk_begin(nk_ctx, TextFormat("PANE(%lld)", (long long)node), nk_rect(rect.x, rect.y, rect.width, rect.height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
      ecs_iter_t it = ecs_children(ecs_ctx, node);
      while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
          ecs_entity_t child = it.entities[i];
          gui_build_render_graph(ecs_ctx, nk_ctx, rect, child);
        }
      }
    }
    nk_end(nk_ctx);
  } else if (gui_type == GUI_NODE_PANE) {
    if (!label || !layout || !anchor) {
      printf("Invalid GuiNodePane\n");
      return;
    }
    int padding = 0;
    Rectangle rect = container;
    //Color color = RED;
    //if (ecs_has_id(ecs_ctx, node, GuiFocus)) {
    //  color = BLACK;
    //}
    //char text[64] = { 0 };
    //char *anchor_text = "GUI_ANCHOR_UNKNOWN";
    //if (anchor) {
    //  switch (*anchor) {
    //  case GUI_ANCHOR_ROOT:
    //    anchor_text = "GUI_ANCHOR_ROOT";
    //    break;
    //  case GUI_ANCHOR_A:
    //    anchor_text = "GUI_ANCHOR_A";
    //    break;
    //  case GUI_ANCHOR_B:
    //    anchor_text = "GUI_ANCHOR_B";
    //    break;
    //  default:
    //    break;
    //  }
    //}
    //sprintf(text, "Pane %lld(%i,%i) %s", (long long)node, layout->xoff, layout->yoff, anchor_text);
    //int text_size = MeasureText(text, 14);
    //DrawText(text, rect.x + (rect.width / 2) - (text_size / 2), rect.y + (rect.height / 2) - 8, 14, color);
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_build_render_graph(ecs_ctx, nk_ctx, rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_SCROLL_VIEW) {
    Rectangle rect = container;
    static bool editmode = false;
    static Vector2 scroll = { .x = 100.0, .y = 100.0 };
    Rectangle scroll_view = { 0 };
    Rectangle content_view = { .x = rect.x, .y = rect.y, .width = rect.width * 2, .height = rect.width * 2 };
    /*
       //Rectangle scroll_view = GuiScrollPanel(rect, NULL, content_view, &scroll);
       if (GuiScrollPanelOwning(rect, NULL, content_view, &scroll, &scroll_view, editmode)) {
       editmode = !editmode;
       }
       BeginScissorMode(scroll_view.x, scroll_view.y, scroll_view.width, scroll_view.height);
       GuiGrid((Rectangle) { rect.x + scroll.x, rect.y + scroll.y, 1000.0, 1000.0 }, NULL, 16, 3);
       EndScissorMode();
     */
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_build_render_graph(ecs_ctx, nk_ctx, rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_VSPLIT) {
    if (!size || !layout || !anchor) {
      printf("Invalid GuiNodeVSplit\n");
      return;
    }
    Rectangle child_a_rect = container;
    child_a_rect.width = (int)((float)container.width * size->value.dynamic.width);

    Rectangle child_b_rect = container;
    child_b_rect.width = (int)((float)container.width * (1.0 - size->value.dynamic.width));
    child_b_rect.x += child_a_rect.width;

    //DrawLineEx((Vector2) { child_b_rect.x, child_b_rect.y }, (Vector2) { child_b_rect.x, child_b_rect.y + child_b_rect.height }, 2, (Color) { 132, 173, 183, 255 });

    //char text[64] = { 0 };
    //char *anchor_text = "GUI_ANCHOR_UNKNOWN";
    //switch (*anchor) {
    //case GUI_ANCHOR_ROOT:
    //  anchor_text = "GUI_ANCHOR_ROOT";
    //  break;
    //case GUI_ANCHOR_A:
    //  anchor_text = "GUI_ANCHOR_A";
    //  break;
    //case GUI_ANCHOR_B:
    //  anchor_text = "GUI_ANCHOR_B";
    //  break;
    //default:
    //  break;
    //}
    //sprintf(text, "VSplit %lld(%i,%i) %s", (long long)node, layout->xoff, layout->yoff, anchor_text);
    //int text_size = MeasureText(text, 14);
    //DrawText(text, child_b_rect.x - (text_size / 2), child_b_rect.y + (child_b_rect.height / 2) - 8, 14, RED);

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
      gui_build_render_graph(ecs_ctx, nk_ctx, child_a_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, nk_ctx, child_b_rect, child_b);
    } else {
      ecs_entity_t child_a = children[0];
      gui_build_render_graph(ecs_ctx, nk_ctx, child_b_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, nk_ctx, child_a_rect, child_b);
    }
  } else if (gui_type == GUI_NODE_HSPLIT) {
    if (!size || !layout || !anchor) {
      printf("Invalid GuiNodeHSplit\n");
      return;
    }
    Rectangle child_a_rect = container;
    child_a_rect.height = (int)((float)container.height * size->value.dynamic.height);
    Rectangle child_b_rect = container;
    child_b_rect.height = (int)((float)container.height * (1.0 - size->value.dynamic.height));
    child_b_rect.y += child_a_rect.height;

    //DrawLineEx((Vector2) { child_b_rect.x, child_b_rect.y }, (Vector2) { child_b_rect.x + child_b_rect.width, child_b_rect.y }, 2, (Color) { 132, 173, 183, 255 });

    //char text[64] = { 0 };
    //char *anchor_text = "GUI_ANCHOR_UNKNOWN";
    //switch (*anchor) {
    //case GUI_ANCHOR_ROOT:
    //  anchor_text = "GUI_ANCHOR_ROOT";
    //  break;
    //case GUI_ANCHOR_A:
    //  anchor_text = "GUI_ANCHOR_A";
    //  break;
    //case GUI_ANCHOR_B:
    //  anchor_text = "GUI_ANCHOR_B";
    //  break;
    //default:
    //  break;
    //}
    //sprintf(text, "HSplit %lld(%i,%i) %s", (long long) node, layout->xoff, layout->yoff, anchor_text);
    //int text_size = MeasureText(text, 14);
    //DrawText(text, child_b_rect.x + (child_b_rect.width / 2) - (text_size / 2), child_b_rect.y - 8, 14, RED);

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
      gui_build_render_graph(ecs_ctx, nk_ctx, child_a_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, nk_ctx, child_b_rect, child_b);
    } else {
      ecs_entity_t child_a = children[0];
      gui_build_render_graph(ecs_ctx, nk_ctx, child_b_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, nk_ctx, child_a_rect, child_b);
    }
  } else if (gui_type == GUI_NODE_ROW) {
    if (!size) {
      printf("Invalid GuiNodeRow\n");
      return;
    }
    Rectangle rect = container;
    if (size->type == GuiDimensionTypeFixed) {
      rect.height = size->value.fixed.height;
    } else {
      rect.height = (int)((float)container.height * size->value.dynamic.height);
    }
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_build_render_graph(ecs_ctx, nk_ctx, rect, child);
      }
    }
  } else if (gui_type == GUI_NODE_TEXTBOX) {
    //static char textBoxText[64];
    //static int textBoxEditMode;
    //if (GuiTextBox(container, textBoxText, 64, textBoxEditMode)) {
    //  textBoxEditMode = !textBoxEditMode;
    //}
  } else if (gui_type == GUI_NODE_DROPDOWN) {
    const gui_event_handle_t *handle = ecs_get(ecs_ctx, node, gui_event_handle_t);
    if (!list || !size || !state) {
      printf("Invalid GuiNodeDropDown\n");
      return;
    }
    int width = 0;
    int height = 0;
    if (size->type == GuiDimensionTypeFixed) {
      width = size->value.fixed.width;
      height = size->value.fixed.height;
    } else {
      width = (int)((float)container.width * size->value.dynamic.width);
      height = (int)((float)container.height * size->value.dynamic.height);
    }
    int mode = state->mode;
    Rectangle rect = {
      .x = container.x + container.width - width,
      .y = container.y,
      .width = width,
      .height = height
    };
    if (nk_begin(nk_ctx, TextFormat("DROPDOWN(%lld)", (long long)node), nk_rect(rect.x, rect.y, rect.width, rect.height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {
      static const char *weapons[] = { "Fist", "Pistol", "Shotgun", "Plasma", "BFG" };
      nk_layout_row_static(nk_ctx, rect.height, rect.width, 1);
      const char **items = (const char**)list->items;
      int new_mode = nk_combo(nk_ctx, items, list->num_items, state->mode, rect.height, nk_vec2(rect.width, 200));
      if (new_mode != state->mode) {
        gui_state_t new_state = { .mode = new_mode };
        if (handle) {
          handle->handle(ecs_ctx, node, &new_state);
        }
        ecs_set_ptr(ecs_ctx, node, gui_state_t, &new_state);
      }
    }
    nk_end(nk_ctx);
  } else if (gui_type == GUI_NODE_SCENE) {
    node_id_t camera_node = ecs_get_target(ecs_ctx, node, ActiveCameraFor, 0);
    const threed_render_texture_t *render_texture = ecs_get(ecs_ctx, camera_node, threed_render_texture_t);
    if (!render_texture) {
      printf("Invalid Camera\n");
      return;
    }
    float padding = 0.0;
    //Rectangle source = { .x = 0.0, .y = 0.0, .width = 1280.0, .height = -720.0 };
    //DrawTexturePro(render_texture->texture.texture, source, dest, (Vector2) { .x = 0.0, .y = 0.0 }, 0.0, WHITE);
    Rectangle dest = { .x = container.x + padding / 2.0, .y = container.y + padding / 2.0, .width = container.width - padding, .height = container.height - padding };
    nk_layout_row_static(nk_ctx, container.height, container.width, 1);
    struct nk_image img = nk_image_id(render_texture->texture.texture.id);
    nk_image(nk_ctx, img);
  } else {
  }
}

/* gui_node_update_system */
void
gui_node_update_system(
  ecs_iter_t *it
) {
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  for (int i = 0; i < it->count; i++) {
    gui_build_render_graph(it->world, nk_ctx, (Rectangle) { 0, 0, GetRenderWidth(), GetRenderHeight() }, it->entities[i]);
  }
}

/* gui_node_render_system */
void
gui_node_render_system(
  ecs_iter_t *it
) {
  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(it->world, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  nk_raylib_draw(nk_ctx);
}
