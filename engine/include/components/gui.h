#ifndef INKIMERA_GUI_H
#define INKIMERA_GUI_H

/*
 * CORE
 */

#define GUI_MAX_LIST_ITEMS 25

/* GuiRoot */
extern ECS_TAG_DECLARE(GuiRoot);

/* GuiFocus */
extern ECS_TAG_DECLARE(GuiFocus);

/* gui_node_type_t */
typedef enum {
  GUI_NODE_EMPTY,
  GUI_NODE_WINDOW,
  GUI_NODE_CONTAINER,
  GUI_NODE_PANE,
  GUI_NODE_VSPLIT,
  GUI_NODE_HSPLIT,
  GUI_NODE_SCROLL_VIEW,
  GUI_NODE_ROW,
  GUI_NODE_COLUMN,
  GUI_NODE_BUTTON,
  GUI_NODE_TEXTBOX,
  GUI_NODE_DROPDOWN,
  GUI_NODE_SCENE
} gui_node_type_t;
extern ECS_COMPONENT_DECLARE(gui_node_type_t);

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

/* gui_list_t */
typedef struct {
  char **items;
  int num_items;
} gui_list_t;
extern ECS_COMPONENT_DECLARE(gui_list_t);

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

/* gui_button_handle_t */
typedef void (*gui_handle_t)(
  ecs_world_t *ecs_ctx,
  ecs_entity_t node,
  gui_state_t *state
);

typedef struct {
  gui_handle_t handle;
} gui_event_handle_t;
extern ECS_COMPONENT_DECLARE(gui_event_handle_t);
#endif
