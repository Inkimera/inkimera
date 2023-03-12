#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"
#include "raygui.h"

#include "engine.h"
#include "components/gui_components.h"
#include "systems/gui_systems.h"
#include "components/3d_components.h"

/* gui_system_init */
int
gui_systems_init(
  engine_t *eng
) {
  ECS_SYSTEM(eng->ecs_ctx, gui_node_render_system, EcsOnStore, GuiNodeWindow);
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

/* gui_build_render_graph */
void
gui_build_render_graph(
  ecs_world_t *ecs_ctx,
  Rectangle container,
  ecs_entity_t node
) {
  //printf("Render Node(%lld)\n", (long long)node);
  //if (!ecs_is_alive(ecs_ctx, node)) {
  //  return;
  //}
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
  const gui_label_t *label = ecs_get(ecs_ctx, node, gui_label_t);
  const gui_position_t *position = ecs_get(ecs_ctx, node, gui_position_t);
  const gui_size_t *size = ecs_get(ecs_ctx, node, gui_size_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
  const gui_state_t *state = ecs_get(ecs_ctx, node, gui_state_t);
  if (ecs_has_id(ecs_ctx, node, GuiNodeWindow)) {
    if (!label || !position || !size) {
      printf("Invalid GuiNodeWindow\n");
      return;
    }
    int padding = 8;
    Rectangle rect, inner_rect;
    rect_with_bounds(&rect, &inner_rect, container, position, size, padding);
    GuiPanel(rect, NULL);
    GuiPanel(inner_rect, NULL);
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_build_render_graph(ecs_ctx, inner_rect, child);
      }
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodePane)) {
    if (!label || !layout || !anchor) {
      printf("Invalid GuiNodePane\n");
      return;
    }
    int padding = 0;
    Rectangle rect = container;
    Color color = RED;
    if (ecs_has_id(ecs_ctx, node, GuiFocus)) {
      color = BLACK;
    }
    char text[64] = { 0 };
    char *anchor_text = "GUI_ANCHOR_UNKNOWN";
    if (anchor) {
      switch (*anchor) {
      case GUI_ANCHOR_ROOT:
        anchor_text = "GUI_ANCHOR_ROOT";
        break;
      case GUI_ANCHOR_A:
        anchor_text = "GUI_ANCHOR_A";
        break;
      case GUI_ANCHOR_B:
        anchor_text = "GUI_ANCHOR_B";
        break;
      default:
        break;
      }
    }
    sprintf(text, "Pane %lld(%i,%i) %s", (long long)node, layout->xoff, layout->yoff, anchor_text);
    int text_size = MeasureText(text, 14);
    DrawText(text, rect.x + (rect.width / 2) - (text_size / 2), rect.y + (rect.height / 2) - 8, 14, color);
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_build_render_graph(ecs_ctx, rect, child);
      }
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeVSplit)) {
    if (!size || !layout || !anchor) {
      printf("Invalid GuiNodeVSplit\n");
      return;
    }
    Rectangle child_a_rect = container;
    child_a_rect.width = (int)((float)container.width * size->value.dynamic.width);

    Rectangle child_b_rect = container;
    child_b_rect.width = (int)((float)container.width * (1.0 - size->value.dynamic.width));
    child_b_rect.x += child_a_rect.width;

    DrawLineEx((Vector2) { child_b_rect.x, child_b_rect.y }, (Vector2) { child_b_rect.x, child_b_rect.y + child_b_rect.height }, 2, (Color) { 132, 173, 183, 255 });

    char text[64] = { 0 };
    char *anchor_text = "GUI_ANCHOR_UNKNOWN";
    switch (*anchor) {
    case GUI_ANCHOR_ROOT:
      anchor_text = "GUI_ANCHOR_ROOT";
      break;
    case GUI_ANCHOR_A:
      anchor_text = "GUI_ANCHOR_A";
      break;
    case GUI_ANCHOR_B:
      anchor_text = "GUI_ANCHOR_B";
      break;
    default:
      break;
    }
    sprintf(text, "VSplit %lld(%i,%i) %s", (long long)node, layout->xoff, layout->yoff, anchor_text);
    int text_size = MeasureText(text, 14);
    DrawText(text, child_b_rect.x - (text_size / 2), child_b_rect.y + (child_b_rect.height / 2) - 8, 14, RED);

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
      gui_build_render_graph(ecs_ctx, child_a_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, child_b_rect, child_b);
    } else {
      ecs_entity_t child_a = children[0];
      gui_build_render_graph(ecs_ctx, child_b_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, child_a_rect, child_b);
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeHSplit)) {
    if (!size || !layout || !anchor) {
      printf("Invalid GuiNodeHSplit\n");
      return;
    }
    Rectangle child_a_rect = container;
    child_a_rect.height = (int)((float)container.height * size->value.dynamic.height);
    Rectangle child_b_rect = container;
    child_b_rect.height = (int)((float)container.height * (1.0 - size->value.dynamic.height));
    child_b_rect.y += child_a_rect.height;

    DrawLineEx((Vector2) { child_b_rect.x, child_b_rect.y }, (Vector2) { child_b_rect.x + child_b_rect.width, child_b_rect.y }, 2, (Color) { 132, 173, 183, 255 });

    char text[64] = { 0 };
    char *anchor_text = "GUI_ANCHOR_UNKNOWN";
    switch (*anchor) {
    case GUI_ANCHOR_ROOT:
      anchor_text = "GUI_ANCHOR_ROOT";
      break;
    case GUI_ANCHOR_A:
      anchor_text = "GUI_ANCHOR_A";
      break;
    case GUI_ANCHOR_B:
      anchor_text = "GUI_ANCHOR_B";
      break;
    default:
      break;
    }
    sprintf(text, "HSplit %lld(%i,%i) %s", (long long) node, layout->xoff, layout->yoff, anchor_text);
    int text_size = MeasureText(text, 14);
    DrawText(text, child_b_rect.x + (child_b_rect.width / 2) - (text_size / 2), child_b_rect.y - 8, 14, RED);

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
      gui_build_render_graph(ecs_ctx, child_a_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, child_b_rect, child_b);
    } else {
      ecs_entity_t child_a = children[0];
      gui_build_render_graph(ecs_ctx, child_b_rect, child_a);

      ecs_entity_t child_b = children[1];
      gui_build_render_graph(ecs_ctx, child_a_rect, child_b);
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeRow)) {
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
        gui_build_render_graph(ecs_ctx, rect, child);
      }
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeTextBox)) {
    static char textBoxText[64];
    static int textBoxEditMode;
    if (GuiTextBox(container, textBoxText, 64, textBoxEditMode)) {
      textBoxEditMode = !textBoxEditMode;
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeDropDown)) {
    if (!label || !size || !state) {
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
    if (GuiDropdownBox(
      (Rectangle) {
      container.x + container.width - width,
      container.y, width, height
      }, *label, &mode, state->active)) {
      ecs_set(ecs_ctx, (ecs_entity_t)node, gui_state_t, { .mode = mode, .active = !state->active });
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeScene)) {
    const threed_render_texture_t *render_texture = ecs_get(ecs_ctx, node, threed_render_texture_t);
    Rectangle source = { .x = 0.0, .y = 0.0, .width = 1280.0, .height = -720.0 };
    Rectangle dest = { .x = 0.0, .y = 0.0, .width = container.width - container.x, .height = container.height - container.y };
    //DrawTextureRec(render_texture->texture.texture, rect, (Vector2) { .x = container.x, .y = container.y }, WHITE);
    DrawTexturePro(render_texture->texture.texture, source, container, (Vector2) { .x = 0.0, .y = 0.0 }, 0.0, WHITE);
  } else {
  }
}

/* gui_node_render_system */
void
gui_node_render_system(
  ecs_iter_t *it
) {
  for (int i = 0; i < it->count; i++) {
    gui_build_render_graph(it->world, (Rectangle) { 0, 0, GetRenderWidth(), GetRenderHeight() }, it->entities[i]);
  }
}
