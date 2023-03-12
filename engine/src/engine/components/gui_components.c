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
ECS_TAG_DECLARE(GuiNodeDropDown);
ECS_TAG_DECLARE(GuiNodeScene);

/* GuiFocus */
ECS_TAG_DECLARE(GuiFocus);

/* gui_anchor_t */
ECS_COMPONENT_DECLARE(gui_anchor_t);

/* gui_label_t */
ECS_COMPONENT_DECLARE(gui_label_t);

/* gui_position_t */
ECS_COMPONENT_DECLARE(gui_position_t);

/* gui_size_t */
ECS_COMPONENT_DECLARE(gui_size_t);

/* gui_layout_t */
ECS_COMPONENT_DECLARE(gui_layout_t);

/* gui_state_t */
ECS_COMPONENT_DECLARE(gui_state_t);

/* gui_components_init */
int
gui_components_init(
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
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeDropDown);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiNodeScene);
  ECS_TAG_DEFINE(eng->ecs_ctx, GuiFocus);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_anchor_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_label_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_position_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_size_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_layout_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, gui_state_t);
  return 0;
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
  ecs_remove(ecs_ctx, node, GuiNodeDropDown);
  ecs_remove(ecs_ctx, node, GuiNodeScene);
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
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeDropDown)) {
    return GUI_NODE_DROPDOWN;
  } else if (ecs_has_id(ecs_ctx, node, GuiNodeScene)) {
    return GUI_NODE_SCENE;
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
  case GUI_NODE_DROPDOWN:
    ecs_add(eng->ecs_ctx, node, GuiNodeDropDown);
    break;
  case GUI_NODE_SCENE:
    ecs_add(eng->ecs_ctx, node, GuiNodeScene);
    break;
  default:
    ecs_add(eng->ecs_ctx, node, GuiNodeEmpty);
    break;
  }
  return 0;
}
