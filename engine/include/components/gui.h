#ifndef INKIMERA_GUI_H
#define INKIMERA_GUI_H
#include "flecs.h"

/*
 * CORE
 */

/* GuiNode */
extern ECS_TAG_DECLARE(GuiNodeEmpty);
extern ECS_TAG_DECLARE(GuiNodeWindow);
extern ECS_TAG_DECLARE(GuiNodePane);
extern ECS_TAG_DECLARE(GuiNodeVSplit);
extern ECS_TAG_DECLARE(GuiNodeHSplit);
extern ECS_TAG_DECLARE(GuiNodeRow);
extern ECS_TAG_DECLARE(GuiNodeColumn);
extern ECS_TAG_DECLARE(GuiNodeButton);
extern ECS_TAG_DECLARE(GuiNodeTextBox);
extern ECS_TAG_DECLARE(GuiNodeDropDown);
extern ECS_TAG_DECLARE(GuiNodeScene);
/* GuiFocus */
extern ECS_TAG_DECLARE(GuiFocus);

/* gui_node_type_t */
typedef enum {
  GUI_NODE_EMPTY,
  GUI_NODE_WINDOW,
  GUI_NODE_PANE,
  GUI_NODE_VSPLIT,
  GUI_NODE_HSPLIT,
  GUI_NODE_ROW,
  GUI_NODE_COLUMN,
  GUI_NODE_BUTTON,
  GUI_NODE_TEXTBOX,
  GUI_NODE_DROPDOWN,
  GUI_NODE_SCENE
} gui_node_type_t;

/* gui_anchor_t */
typedef enum {
  GUI_ANCHOR_ROOT,
  GUI_ANCHOR_A,
  GUI_ANCHOR_B,
  GUI_ANCHOR_UNKNOWN
} gui_anchor_t;
extern ECS_COMPONENT_DECLARE(gui_anchor_t);

/* gui_dimension_type_t */
typedef enum {
  GuiDimensionTypeFixed,
  GuiDimensionTypeDynamic,
} gui_dimension_type_t;
extern ECS_COMPONENT_DECLARE(gui_dimension_type_t);

/* gui_label_t */
typedef char*gui_label_t;
extern ECS_COMPONENT_DECLARE(gui_label_t);

/* gui_position_t */
typedef struct {
  gui_dimension_type_t type;
  union {
    struct {
      int x;
      int y;
    } fixed;
    struct {
      float x;
      float y;
    } dynamic;
  } value;
} gui_position_t;
extern ECS_COMPONENT_DECLARE(gui_position_t);

/* gui_size_t */
typedef struct {
  gui_dimension_type_t type;
  union {
    struct {
      int width;
      int height;
    } fixed;
    struct {
      float width;
      float height;
    } dynamic;
  } value;
} gui_size_t;
extern ECS_COMPONENT_DECLARE(gui_size_t);

/* gui_layout_t */
typedef struct {
  int columns;
  int xoff;
  int yoff;
} gui_layout_t;
extern ECS_COMPONENT_DECLARE(gui_layout_t);

/* gui_state_t */
typedef struct {
  int mode;
  bool active;
} gui_state_t;
extern ECS_COMPONENT_DECLARE(gui_state_t);
#endif
