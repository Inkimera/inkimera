#ifndef INKIMERA_GUI_H
#define INKIMERA_GUI_H
#include "flecs.h"

/*
 * CORE
 */

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
  GUI_NODE_VI
} gui_node_type_t;

/* gui_node_anchor_t */
typedef enum {
  GUI_ANCHOR_ROOT,
  GUI_ANCHOR_A,
  GUI_ANCHOR_B,
  GUI_ANCHOR_UNKNOWN
} gui_node_anchor_t;

/* gui_dimension_type_t */
typedef enum {
  GuiDimensionTypeFixed,
  GuiDimensionTypeDynamic,
} gui_dimension_type_t;

/* gui_label_t */
typedef char*gui_label_t;

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
  };
} gui_position_t;

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
  };
} gui_size_t;

/* gui_layout_t */
typedef struct {
  int columns;
  int xoff;
  int yoff;
} gui_layout_t;
#endif
