#ifndef GUI_NODE_H
#define GUI_NODE_H

#include "project.h"

#include "inkimera.h"

#define MAX_PATH 1024

extern const struct nk_color INK_BLACK;
extern const struct nk_color INK_BLACK_INK;
extern const struct nk_color INK_WHITE;
extern const struct nk_color INK_GREY;
extern const struct nk_color INK_DARK_GREY;
//extern const struct nk_color INK_LIGHT_BONE;
//extern const struct nk_color INK_BONE;
extern const struct nk_color INK_TRANSPARENT;

/* gui_node_type_t */
typedef enum {
  GUI_NODE_WINDOW,
  GUI_NODE_PANE,
  GUI_NODE_VSPLIT,
  GUI_NODE_HSPLIT,
  GUI_NODE_WIDGET
} gui_node_type_t;
extern ECS_COMPONENT_DECLARE(gui_node_type_t);

/* gui_widget_type_t */
typedef enum {
  GUI_WIDGET_SCENE,
  GUI_WIDGET_TERM,
  GUI_WIDGET_PROPS,
  GUI_WIDGET_SCENE_GRAPH,
  GUI_WIDGET_MENU_BAR
} gui_widget_type_t;
extern ECS_COMPONENT_DECLARE(gui_widget_type_t);

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
  unsigned long target;
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

/* gui_hash */
unsigned long
gui_hash(
  const char *str
);

/* gui_get_homedir */
int
gui_get_homedir(
  char *homedir
);

/* gui_new_window */
node_id_t
gui_new_window(
  engine_t *eng,
  int x,
  int y,
  float height,
  float width,
  char *label
);

/* gui_new_pane */
node_id_t
gui_new_pane(
  engine_t *eng,
  char *label,
  node_id_t parent
);

/* gui_new_vsplit */
node_id_t
gui_new_vsplit(
  engine_t *eng,
  node_id_t parent
);

/* gui_new_hsplit */
node_id_t
gui_new_hsplit(
  engine_t *eng,
  node_id_t parent
);

/* gui_new_button */
node_id_t
gui_new_button(
  engine_t *eng,
  char *label,
  node_id_t parent
);

/* gui_new_textbox */
node_id_t
gui_new_textbox(
  engine_t *eng,
  node_id_t parent
);

/* gui_new_dropdown */
node_id_t
gui_new_dropdown(
  engine_t *eng,
  node_id_t parent,
  char **items,
  int num_items,
  int width,
  int height
);

/* gui_new_widget */
node_id_t
gui_new_widget(
  ecs_world_t *ecs_ctx,
  node_id_t parent,
  gui_widget_type_t widget_type
);

/* gui_project */
void
gui_project(
  ecs_world_t *ecs_ctx,
  node_id_t window,
  project_t project
);
#endif
