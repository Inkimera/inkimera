#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"
#include "raygui.h"

#include "engine.h"
#include "entities/gui_node.h"

/* script_state_t */
ECS_COMPONENT_DECLARE(script_state_t);

/* script_t */
ECS_COMPONENT_DECLARE(script_t);

/*
 * GUI CORE
 */

/* GuiNode */
ECS_TAG_DECLARE(GuiNodeEmpty);
ECS_TAG_DECLARE(GuiNodeWindow);
ECS_TAG_DECLARE(GuiNodePane);
ECS_TAG_DECLARE(GuiNodeVSplit);
ECS_TAG_DECLARE(GuiNodeHSplit);
ECS_TAG_DECLARE(GuiNodeRow);
ECS_TAG_DECLARE(GuiNodeColumn);
ECS_TAG_DECLARE(GuiNodeButton);
ECS_TAG_DECLARE(GuiNodeTextBox);
ECS_TAG_DECLARE(GuiNodeVI);

/* GuiFocus */
ECS_TAG_DECLARE(GuiFocus);

/* gui_node_anchor_t */
ECS_COMPONENT_DECLARE(gui_node_anchor_t);

/* gui_label_t */
ECS_COMPONENT_DECLARE(gui_label_t);

/* gui_position_t */
ECS_COMPONENT_DECLARE(gui_position_t);

/* gui_size_t */
ECS_COMPONENT_DECLARE(gui_size_t);

/* gui_layout_t */
ECS_COMPONENT_DECLARE(gui_layout_t);

/* gui_system_init */
int
gui_system_init(
  engine_t *eng
) {
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, script_t);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeEmpty);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeWindow);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodePane);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeVSplit);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeHSplit);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeRow);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeColumn);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeButton);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeTextBox);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeVI);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiFocus);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_node_anchor_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_label_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_position_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_size_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_layout_t);
  ECS_SYSTEM(eng->ecs_ctx, gui_node_system, EcsOnUpdate, GuiNodeWindow);
  return 0;
}

/* gui_node_init */
node_id_t
gui_node_init(
  engine_t *eng
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_entity_t node = ecs_new_id(ecs_ctx);
  printf("Init GuiNode %lld\n", (long long)node);
  ecs_add(ecs_ctx, node, GuiNodeEmpty);
  return node;
}

/* gui_node_clear */
int
gui_node_clear(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_remove(ecs_ctx, node, GuiNodeEmpty);
  ecs_remove(ecs_ctx, node, GuiNodeWindow);
  ecs_remove(ecs_ctx, node, GuiNodePane);
  ecs_remove(ecs_ctx, node, GuiNodeVSplit);
  ecs_remove(ecs_ctx, node, GuiNodeHSplit);
  ecs_remove(ecs_ctx, node, GuiNodeRow);
  ecs_remove(ecs_ctx, node, GuiNodeColumn);
  ecs_remove(ecs_ctx, node, GuiNodeButton);
  ecs_remove(ecs_ctx, node, GuiNodeTextBox);
  ecs_remove(ecs_ctx, node, GuiNodeVI);
  return 0;
}

/* gui_node_get_type */
gui_node_type_t
gui_node_get_type(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  if (ecs_has_id(ecs_ctx, node, GuiNodeWindow)) {
    return GUI_NODE_WINDOW;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodePane)) {
    return GUI_NODE_PANE;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeVSplit)) {
    return GUI_NODE_VSPLIT;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeHSplit)) {
    return GUI_NODE_HSPLIT;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeRow)) {
    return GUI_NODE_ROW;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeColumn)) {
    return GUI_NODE_COLUMN;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeButton)) {
    return GUI_NODE_BUTTON;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeTextBox)) {
    return GUI_NODE_TEXTBOX;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeVI)) {
    return GUI_NODE_VI;
  } else {
    return GUI_NODE_EMPTY;
  }
}

/* gui_node_set_type */
int
gui_node_set_type(
  engine_t *eng,
  node_id_t node,
  gui_node_type_t type
) {
  gui_node_clear(eng, node);
  switch (type) {
  case GUI_NODE_WINDOW:
    ecs_add(eng->ecs_ctx, node, GuiNodeWindow);
    break;
  case GUI_NODE_PANE:
    ecs_add(eng->ecs_ctx, node, GuiNodePane);
    break;
  case GUI_NODE_VSPLIT:
    ecs_add(eng->ecs_ctx, node, GuiNodeVSplit);
    break;
  case GUI_NODE_HSPLIT:
    ecs_add(eng->ecs_ctx, node, GuiNodeHSplit);
    break;
  case GUI_NODE_ROW:
    ecs_add(eng->ecs_ctx, node, GuiNodeRow);
    break;
  case GUI_NODE_COLUMN:
    ecs_add(eng->ecs_ctx, node, GuiNodeColumn);
    break;
  case GUI_NODE_BUTTON:
    ecs_add(eng->ecs_ctx, node, GuiNodeButton);
    break;
  case GUI_NODE_TEXTBOX:
    ecs_add(eng->ecs_ctx, node, GuiNodeTextBox);
    break;
  case GUI_NODE_VI:
    ecs_add(eng->ecs_ctx, node, GuiNodeVI);
    break;
  default:
    ecs_add(eng->ecs_ctx, node, GuiNodeEmpty);
    break;
  }
  return 0;
}

/* gui_focus */
void
gui_node_focus(
  engine_t *eng,
  node_id_t node
) {
  ecs_add(eng->ecs_ctx, node, GuiFocus);
}

/* gui_unfocus */
void
gui_node_unfocus(
  engine_t *eng,
  node_id_t node
) {
  ecs_remove(eng->ecs_ctx, node, GuiFocus);
}

/* gui_node_get_anchor */
gui_node_anchor_t
gui_node_get_anchor(
  engine_t *eng,
  node_id_t node
) {
  const gui_node_anchor_t *anchor
    = ecs_get(eng->ecs_ctx, node, gui_node_anchor_t);
  if (anchor) {
    return *anchor;
  } else {
    return GUI_ANCHOR_UNKNOWN;
  }
}

/* gui_node_set_anchor */
int
gui_node_set_anchor(
  engine_t *eng,
  node_id_t node,
  gui_node_anchor_t anchor
) {
  return ecs_set_ptr(eng->ecs_ctx, node, gui_node_anchor_t, &anchor);
}

/* gui_node_get_label */
const gui_label_t
gui_node_get_label(
  engine_t *eng,
  node_id_t node
) {
  const gui_label_t *label = ecs_get(eng->ecs_ctx, node, gui_label_t);
  if (!label) {
    return (gui_label_t)"";
  } else {
    return *label;
  }
}

/* gui_node_set_label */
int
gui_node_set_label(
  engine_t *eng,
  node_id_t node,
  gui_label_t label
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_set_ptr(ecs_ctx, (ecs_entity_t)node, gui_label_t, &label);
  return 0;
}

/* gui_node_set_position */
int
gui_node_set_position(
  engine_t *eng,
  node_id_t node,
  gui_position_t position
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_set_ptr(ecs_ctx, (ecs_entity_t)node, gui_position_t, &position);
  return 0;
}

/* gui_node_get_size */
const gui_size_t*
gui_node_get_size(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  return ecs_get(ecs_ctx, node, gui_size_t);
}

/* gui_node_set_size */
int
gui_node_set_size(
  engine_t *eng,
  node_id_t node,
  gui_size_t size
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_set_ptr(ecs_ctx, (ecs_entity_t)node, gui_size_t, &size);
  return 0;
}

/* gui_node_get_layout */
const gui_layout_t*
gui_node_get_layout(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  return ecs_get(ecs_ctx, node, gui_layout_t);
}

/* gui_node_set_layout */
int
gui_node_set_layout(
  engine_t *eng,
  node_id_t node,
  gui_layout_t layout
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_set_ptr(ecs_ctx, (ecs_entity_t)node, gui_layout_t, &layout);
  return 0;
}

/*
 * RENDER
 */

/* rect_with_bounds */
void
rect_with_bounds(
  Rectangle *rect,
  Rectangle *inner_rect,
  Rectangle container,
  const gui_position_t *position,
  const gui_size_t *size,
  int padding
) {
  if (position->type == GuiDimensionTypeFixed) {
    rect->x = position->fixed.x;
    rect->y = position->fixed.y;
  } else {
    rect->x =
      (int)((float)GetRenderWidth() * position->dynamic.x);
    rect->y =
      (int)((float)GetRenderHeight() * position->dynamic.y);
  }
  if (size->type == GuiDimensionTypeFixed) {
    rect->width = size->fixed.width;
    rect->height = size->fixed.height;
  } else {
    rect->width = (int)((float)GetRenderWidth() * size->dynamic.width);
    rect->height = (int)((float)GetRenderHeight() * size->dynamic.height);
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
  const gui_node_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_node_anchor_t);
  const gui_label_t *label = ecs_get(ecs_ctx, node, gui_label_t);
  const gui_position_t *position = ecs_get(ecs_ctx, node, gui_position_t);
  const gui_size_t *size = ecs_get(ecs_ctx, node, gui_size_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
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
    //if (!label) {
    //  printf("Invalid GuiNodePane\n");
    //  return;
    //}
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
    //rect.x += padding;
    //rect.y += padding;
    //rect.width -= padding * 2;
    //rect.height -= padding * 2;
    static int active = 0;
    static int edit = false;
    if (GuiDropdownBox((Rectangle) { rect.x + rect.width - 125, rect.y, 125, 30 }, "SCENE;TERM;PROPS", &active, edit)) {
      edit = !edit;
    }
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        gui_build_render_graph(ecs_ctx, rect, child);
      }
    }
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeVSplit)) {
    if (!size) {
      printf("Invalid GuiNodeVSplit\n");
      return;
    }
    Rectangle child_a_rect = container;
    child_a_rect.width = (int)((float)container.width * size->dynamic.width);

    Rectangle child_b_rect = container;
    child_b_rect.width = (int)((float)container.width * (1.0 - size->dynamic.width));
    child_b_rect.x += child_a_rect.width;

    DrawLineEx((Vector2) { child_b_rect.x, child_b_rect.y }, (Vector2) { child_b_rect.x, child_b_rect.y + child_b_rect.height }, 2, (Color) { 132, 173, 183, 255 });

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
    const gui_node_anchor_t *child_anchor = ecs_get(ecs_ctx, children[0], gui_node_anchor_t);
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
    if (!size) {
      printf("Invalid GuiNodeHSplit\n");
      return;
    }
    Rectangle child_a_rect = container;
    child_a_rect.height = (int)((float)container.height * size->dynamic.height);
    Rectangle child_b_rect = container;
    child_b_rect.height = (int)((float)container.height * (1.0 - size->dynamic.height));
    child_b_rect.y += child_a_rect.height;

    DrawLineEx((Vector2) { child_b_rect.x, child_b_rect.y }, (Vector2) { child_b_rect.x + child_b_rect.width, child_b_rect.y }, 2, (Color) { 132, 173, 183, 255 });

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
    const gui_node_anchor_t *child_anchor = ecs_get(ecs_ctx, children[0], gui_node_anchor_t);
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
      rect.height = size->fixed.height;
    } else {
      rect.height = (int)((float)container.height * size->dynamic.height);
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
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeVI)) {
  } else {
  }
}

/* gui_node_system */
void
gui_node_system(
  ecs_iter_t *it
) {
  for (int i = 0; i < it->count; i++) {
    gui_build_render_graph(it->world, (Rectangle) { 0, 0, GetRenderWidth(), GetRenderHeight() }, it->entities[i]);
  }
}
