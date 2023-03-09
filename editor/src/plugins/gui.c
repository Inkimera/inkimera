#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "inkimera.h"
#include "plugins/gui.h"

const char *GUI_PLUGIN_KEY = "gui_plugin_key";

/*
 * GUI_PLUGIN
 */

/* gui_new_window */
node_id_t
gui_new_window(
  engine_t *eng,
  int x,
  int y,
  float height,
  float width,
  char *label
) {
  node_id_t window = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, window, GUI_NODE_WINDOW);
  engine_gui_node_set_label(eng, window, (gui_label_t)label);
  gui_position_t window_pos = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .x = x, .y = y }
  };
  engine_gui_node_set_position(eng, window, window_pos);
  gui_size_t window_size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .width = width, .height = height }
  };
  engine_gui_node_set_size(eng, window, window_size);
  gui_layout_t window_layout = {};
  engine_gui_node_set_layout(eng, window, window_layout);
  return window;
}

/* gui_new_pane */
node_id_t
gui_new_pane(
  engine_t *eng,
  char *label,
  node_id_t parent
) {
  node_id_t pane = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, pane, GUI_NODE_PANE);
  engine_gui_node_set_label(eng, pane, (gui_label_t)label);
  gui_layout_t pane_layout = {};
  engine_gui_node_set_layout(eng, pane, pane_layout);
  engine_ecs_parent(eng, pane, parent);
  return pane;
}

/* gui_new_vsplit */
node_id_t
gui_new_vsplit(
  engine_t *eng,
  node_id_t parent
) {
  node_id_t split = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, split, GUI_NODE_VSPLIT);
  gui_size_t split_size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .width = 0.5 }
  };
  engine_gui_node_set_size(eng, split, split_size);
  gui_layout_t split_layout = {};
  engine_gui_node_set_layout(eng, split, split_layout);
  engine_ecs_parent(eng, split, parent);
  return split;
}

/* gui_new_hsplit */
node_id_t
gui_new_hsplit(
  engine_t *eng,
  node_id_t parent
) {
  node_id_t split = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, split, GUI_NODE_HSPLIT);
  gui_size_t split_size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .height = 0.5 }
  };
  engine_gui_node_set_size(eng, split, split_size);
  gui_layout_t split_layout = {};
  engine_gui_node_set_layout(eng, split, split_layout);
  engine_ecs_parent(eng, split, parent);
  return split;
}

/* gui_new_row */
node_id_t
gui_new_row(
  engine_t *eng,
  float height,
  int columns,
  node_id_t parent
) {
  node_id_t row = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, row, GUI_NODE_ROW);
  gui_size_t row_size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .height = height }
  };
  engine_gui_node_set_size(eng, row, row_size);
  gui_layout_t row_layout = {
    .columns = columns,
  };
  engine_gui_node_set_layout(eng, row, row_layout);
  engine_ecs_parent(eng, row, parent);
  return row;
}

/* gui_new_column */
node_id_t
gui_new_column(
  engine_t *eng,
  float width,
  node_id_t parent
) {
  node_id_t column = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, column, GUI_NODE_COLUMN);
  gui_size_t column_size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .width = width }
  };
  engine_gui_node_set_size(eng, column, column_size);
  engine_ecs_parent(eng, column, parent);
  return column;
}

/* gui_new_button */
node_id_t
gui_new_button(
  engine_t *eng,
  char *label,
  node_id_t parent
) {
  node_id_t button = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, button, GUI_NODE_BUTTON);
  engine_gui_node_set_label(eng, button, (gui_label_t)label);
  engine_ecs_parent(eng, button, parent);
  return button;
}

/* gui_new_textbox */
node_id_t
gui_new_textbox(
  engine_t *eng,
  node_id_t parent
) {
  node_id_t textbox = engine_gui_node_create(eng);
  engine_gui_node_set_type(eng, textbox, GUI_NODE_TEXTBOX);
  engine_ecs_parent(eng, textbox, parent);
  return textbox;
}

/*
 *
 */

/* gui_search_direction_t */
typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} gui_search_direction_t;

/* gui_get_children */
int
gui_get_children(
  engine_t *eng,
  node_id_t node,
  node_id_t *children
) {
  node_id_t chldrn[2] = { 0 };
  int num_children = engine_ecs_get_children_of(eng, node, chldrn, 2);

  gui_node_anchor_t a_anchor = engine_gui_node_get_anchor(eng, chldrn[0]);
  if (a_anchor == GUI_ANCHOR_A) {
    children[0] = chldrn[0];
    children[1] = chldrn[1];
  } else {
    children[0] = chldrn[1];
    children[1] = chldrn[0];
  }
  return num_children;
}

/* gui_search_for_split */
node_id_t
gui_search_for_split(
  engine_t *eng,
  node_id_t leaf,
  gui_node_type_t type
) {
  node_id_t parent = engine_ecs_get_parent_of(eng, leaf);
  if (parent == 0) {
    return 0;
  } else {
    gui_node_type_t parent_type = engine_gui_node_get_type(eng, parent);
    if (parent_type == type) {
      return parent;
    } else {
      return gui_search_for_split(eng, parent, type);
    }
  }
}

/* gui_set_xoff_yoff */
void
gui_set_xoff_yoff(
  engine_t *eng,
  node_id_t node,
  int xoff,
  int yoff
) {
  gui_layout_t layout = {
    .xoff = xoff,
    .yoff = yoff
  };
  engine_gui_node_set_layout(eng, node, layout);
}

/* gui_search_next_pane */
node_id_t
gui_search_next_pane(
  engine_t *eng,
  node_id_t node,
  gui_search_direction_t direction
) {
  gui_node_type_t type = engine_gui_node_get_type(eng, node);
  if (type == GUI_NODE_PANE) {
    return node;
  } else {
    node_id_t children[2];
    gui_get_children(eng, node, children);
    node_id_t a = children[0];
    node_id_t b = children[1];
    switch (direction) {
    case UP:
      return gui_search_next_pane(eng, b, direction);
    case DOWN:
      return gui_search_next_pane(eng, a, direction);
    case LEFT:
      return gui_search_next_pane(eng, b, direction);
    case RIGHT:
      return gui_search_next_pane(eng, a, direction);
    }
    return node;
  }
}

/* gui_reverse_search_next_pane */
node_id_t
gui_reverse_search_next_pane(
  engine_t *eng,
  node_id_t node,
  gui_search_direction_t direction
) {
  node_id_t parent = engine_ecs_get_parent_of(eng, node);
  if (parent <= 0) {
    return 0;
  }
  node_id_t children[2] = { 0 };
  int num_children = gui_get_children(eng, parent, children);
  if (num_children < 2) {
    return 0;
  }
  node_id_t a = children[0];
  node_id_t b = children[1];

  node_id_t sibling;
  if (a == node) {
    sibling = b;
  } else {
    sibling = a;
  }
  const gui_layout_t *slayout = engine_gui_node_get_layout(eng, sibling);
  switch (direction) {
  case UP:
    if (slayout->xoff == 0 && slayout->yoff == 1) {
      return gui_search_next_pane(eng, sibling, direction);
    } else {
      return gui_reverse_search_next_pane(eng, parent, direction);
    }
  case DOWN:
    if (slayout->xoff == 0 && slayout->yoff == -1) {
      return gui_search_next_pane(eng, sibling, direction);
    } else {
      return gui_reverse_search_next_pane(eng, parent, direction);
    }
  case LEFT:
    if (slayout->xoff == -1 && slayout->yoff == 0) {
      return gui_search_next_pane(eng, sibling, direction);
    } else {
      return gui_reverse_search_next_pane(eng, parent, direction);
    }
  case RIGHT:
    if (slayout->xoff == 1 && slayout->yoff == 0) {
      return gui_search_next_pane(eng, sibling, direction);
    } else {
      return gui_reverse_search_next_pane(eng, parent, direction);
    }
  default:
    return node;
  }
}

/* gui_resize_pane_vertical */
void
gui_resize_pane_vertical(
  engine_t *eng,
  node_id_t node,
  float delta
) {
  node_id_t target = gui_search_for_split(eng, node, GUI_NODE_VSPLIT);
  if (target == 0) {
    return;
  }
  const gui_size_t *old_size
    = engine_gui_node_get_size(eng, target);
  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .width = old_size->dynamic.width + delta, .height = old_size->dynamic.height }
  };
  if (size.dynamic.width < 0.0) {
    size.dynamic.width = 0.0;
  }
  if (size.dynamic.width > 1.0) {
    size.dynamic.width = 1.0;
  }
  engine_gui_node_set_size(eng, target, size);
}

/* gui_resize_pane_horizontal */
void
gui_resize_pane_horizontal(
  engine_t *eng,
  node_id_t node,
  float delta
) {
  node_id_t target = gui_search_for_split(eng, node, GUI_NODE_HSPLIT);
  if (target == 0) {
    return;
  }
  const gui_size_t *old_size
    = engine_gui_node_get_size(eng, target);
  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .dynamic = { .width = old_size->dynamic.width, .height = old_size->dynamic.height + delta }
  };
  if (size.dynamic.height < 0.0) {
    size.dynamic.height = 0.0;
  }
  if (size.dynamic.height > 1.0) {
    size.dynamic.height = 1.0;
  }
  engine_gui_node_set_size(eng, target, size);
}

/* gui_split_pane_vertical */
node_id_t
gui_split_pane_vertical(
  engine_t *eng,
  node_id_t node
) {
  node_id_t parent = engine_ecs_get_parent_of(eng, node);
  node_id_t vsplit = gui_new_vsplit(eng, parent);
  gui_node_anchor_t anchor = engine_gui_node_get_anchor(eng, node);
  engine_gui_node_set_anchor(eng, vsplit, anchor);
  const gui_layout_t *layout = engine_gui_node_get_layout(eng, node);
  engine_gui_node_set_layout(eng, vsplit, *layout);

  node_id_t pane1 = node;
  engine_ecs_parent(eng, pane1, vsplit);
  engine_gui_node_set_anchor(eng, pane1, GUI_ANCHOR_A);
  gui_set_xoff_yoff(eng, pane1, -1, 0);

  node_id_t pane2 = gui_new_pane(eng, "", vsplit);
  engine_gui_node_set_anchor(eng, pane2, GUI_ANCHOR_B);
  gui_set_xoff_yoff(eng, pane2, 1, 0);
  return pane2;
}

/* gui_split_pane_horizontal */
node_id_t
gui_split_pane_horizontal(
  engine_t *eng,
  node_id_t node
) {
  node_id_t parent = engine_ecs_get_parent_of(eng, node);
  node_id_t hsplit = gui_new_hsplit(eng, parent);
  gui_node_anchor_t anchor = engine_gui_node_get_anchor(eng, node);
  engine_gui_node_set_anchor(eng, hsplit, anchor);
  const gui_layout_t *layout = engine_gui_node_get_layout(eng, node);
  engine_gui_node_set_layout(eng, hsplit, *layout);

  node_id_t pane1 = node;
  engine_ecs_parent(eng, pane1, hsplit);
  engine_gui_node_set_anchor(eng, pane1, GUI_ANCHOR_A);
  gui_set_xoff_yoff(eng, pane1, 0, 1);

  node_id_t pane2 = gui_new_pane(eng, "", hsplit);
  engine_gui_node_set_anchor(eng, pane2, GUI_ANCHOR_B);
  gui_set_xoff_yoff(eng, pane2, 0, -1);
  return pane2;
}

/* gui_delete_pane */
node_id_t
gui_delete_pane(
  engine_t *eng,
  node_id_t window,
  node_id_t node
) {
  node_id_t parent = engine_ecs_get_parent_of(eng, node);
  if (parent == 0 || parent == window) {
    return node;
  }
  node_id_t grand_parent = engine_ecs_get_parent_of(eng, parent);
  engine_gui_node_delete(eng, node);
  node_id_t children[1] = { 0 };
  engine_ecs_get_children_of(eng, parent, children, 1);
  gui_node_anchor_t anchor = engine_gui_node_get_anchor(eng, parent);
  engine_gui_node_set_anchor(eng, children[0], anchor);
  const gui_layout_t *layout = engine_gui_node_get_layout(eng, parent);
  engine_gui_node_set_layout(eng, children[0], *layout);

  engine_ecs_parent(eng, children[0], grand_parent);
  engine_gui_node_delete(eng, parent);

  return gui_search_next_pane(eng, children[0], UP);
}

/*
 * GUI CORE
 */

/* gui_plugin_init */
gui_plugin_t*
gui_plugin_init() {
  gui_plugin_t *plug = malloc(sizeof(gui_plugin_t));
  return plug;
}

/* gui_plugin_deinit */
int
gui_plugin_deinit(
  gui_plugin_t *plug
) {
  free(plug);
  return 0;
}

/* gui_plugin_load */
int
gui_plugin_load(
  engine_t *eng,
  void *plug
) {
  printf("LOAD gui plugin\n");

  gui_plugin_t *gui_plug = (gui_plugin_t*)plug;
  node_id_t window = gui_new_window(eng, 0, 0, 1.0, 1.0, "Inkimera Editor");
  node_id_t pane = gui_new_pane(eng, "", window);
  engine_gui_node_set_anchor(eng, pane, GUI_ANCHOR_ROOT);
  engine_gui_node_focus(eng, pane);
  gui_plug->window = window;
  gui_plug->focus = pane;
  gui_plug->timestamp = time(NULL);

  // Setup
  state_machine_state_t *idle_state = state_machine_add_state(&gui_plug->state, "idle");
  state_machine_state_t *leader_key_held_state = state_machine_add_state(&gui_plug->state, "leader_key_held");
  state_machine_state_t *leader_key_released_state = state_machine_add_state(&gui_plug->state, "leader_key_released");
  state_machine_state_t *command_state = state_machine_add_state(&gui_plug->state, "command");

  state_machine_event_t *event_leader_held =
    state_machine_add_event(&gui_plug->state, (int[2]) { ENGINE_KEY_LEFT_CONTROL, ENGINE_KEY_A }, (int[2]) { ENGINE_KEY_STATE_DOWN, ENGINE_KEY_STATE_DOWN }, 2);
  state_machine_event_t *event_leader_released = state_machine_add_event(&gui_plug->state, (int[2]) { ENGINE_KEY_LEFT_CONTROL, ENGINE_KEY_A },
    (int[2]) { ENGINE_KEY_STATE_RELEASED | ENGINE_KEY_STATE_UP, ENGINE_KEY_STATE_RELEASED | ENGINE_KEY_STATE_UP }, 2);
  state_machine_event_t *event_esc = state_machine_add_event(&gui_plug->state, (int[1]) { ENGINE_KEY_ESCAPE }, (int[1]) { ENGINE_KEY_STATE_DOWN | ENGINE_KEY_STATE_PRESSED }, 1);

  state_machine_add_transition(&gui_plug->state, event_leader_held, idle_state, leader_key_held_state);
  state_machine_add_transition(&gui_plug->state, event_leader_released, leader_key_held_state, leader_key_released_state);
  state_machine_add_transition(&gui_plug->state, event_leader_held, leader_key_released_state, leader_key_held_state);
  state_machine_add_transition(&gui_plug->state, event_esc, leader_key_released_state, idle_state);
  return 0;
}

/* gui_plugin_unload */
int
gui_plugin_unload(
  engine_t *eng,
  void *plug
) {
  printf("UNLOAD gui plugin\n");
  gui_plugin_t *gui_plug = (gui_plugin_t*)plug;
  return gui_plugin_deinit(gui_plug);
}

/* gui_plugin_events */
void
gui_plugin_events(
  engine_t *eng,
  void *plug
) {
  gui_plugin_t *gui_plug = (gui_plugin_t*)plug;

  engine_key_state_t escape = engine_key_state_get(ENGINE_KEY_ESCAPE);
  engine_key_state_t ctrl = engine_key_state_get(ENGINE_KEY_LEFT_CONTROL) | engine_key_state_get(ENGINE_KEY_RIGHT_CONTROL);
  engine_key_state_t a = engine_key_state_get(ENGINE_KEY_A);

  time_t now = time(NULL);
  if (strcmp(state_machine_get_state(&gui_plug->state), "idle") != 0 && now - gui_plug->timestamp > 2) {
    state_machine_set_state(&gui_plug->state, "idle");
    gui_plug->timestamp = now;
  }
  int changed_state = state_machine_process(&gui_plug->state, (int[3]) { ENGINE_KEY_ESCAPE, ENGINE_KEY_LEFT_CONTROL, ENGINE_KEY_A }, (int[3]) { escape, ctrl, a }, 3);
  if (changed_state) {
    char *new_state = state_machine_get_state(&gui_plug->state);
    if (strcmp(new_state, "idle") == 0) {
      printf("Entered Idle state\n");
    } else if (strcmp(new_state, "leader_key_held") == 0) {
      printf("Entered LeaderKeyHeld state\n");
    } else if (strcmp(new_state, "leader_key_released") == 0) {
      printf("Entered LeaderKeyReleased state\n");
    }
    gui_plug->timestamp = now;
  } else {
    char *current_state = state_machine_get_state(&gui_plug->state);
    engine_key_state_t up = engine_key_state_get(ENGINE_KEY_UP);
    engine_key_state_t down = engine_key_state_get(ENGINE_KEY_DOWN);
    engine_key_state_t left = engine_key_state_get(ENGINE_KEY_LEFT);
    engine_key_state_t right = engine_key_state_get(ENGINE_KEY_RIGHT);
    if (strcmp(current_state, "idle") == 0) {
    } else if (strcmp(current_state, "leader_key_held") == 0) {
      if (up & ENGINE_KEY_STATE_RELEASED) {
        gui_resize_pane_horizontal(eng, gui_plug->focus, -0.05);
        gui_plug->timestamp = now;
      } else if (down & ENGINE_KEY_STATE_RELEASED) {
        gui_resize_pane_horizontal(eng, gui_plug->focus, 0.05);
        gui_plug->timestamp = now;
      }
      if (left & ENGINE_KEY_STATE_RELEASED) {
        gui_resize_pane_vertical(eng, gui_plug->focus, -0.05);
        gui_plug->timestamp = now;
      } else if (right & ENGINE_KEY_STATE_RELEASED) {
        gui_resize_pane_vertical(eng, gui_plug->focus, 0.05);
        gui_plug->timestamp = now;
      }
    } else if (strcmp(current_state, "leader_key_released") == 0) {
      engine_key_state_t shift
        = engine_key_state_get(ENGINE_KEY_LEFT_SHIFT)
          | engine_key_state_get(ENGINE_KEY_RIGHT_SHIFT);
      engine_key_state_t five
        = engine_key_state_get(ENGINE_KEY_FIVE);
      engine_key_state_t apostrophe
        = engine_key_state_get(ENGINE_KEY_APOSTROPHE);
      engine_key_state_t x
        = engine_key_state_get(ENGINE_KEY_X);
      if (shift & ENGINE_KEY_STATE_DOWN
          && five & ENGINE_KEY_STATE_RELEASED) {
        engine_gui_node_unfocus(eng, gui_plug->focus);
        gui_plug->focus = gui_split_pane_vertical(eng, gui_plug->focus);
        engine_gui_node_focus(eng, gui_plug->focus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (shift & ENGINE_KEY_STATE_DOWN
                 && apostrophe & ENGINE_KEY_STATE_RELEASED) {
        engine_gui_node_unfocus(eng, gui_plug->focus);
        gui_plug->focus = gui_split_pane_horizontal(eng, gui_plug->focus);
        engine_gui_node_focus(eng, gui_plug->focus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (x & ENGINE_KEY_STATE_RELEASED) {
        gui_plug->focus = gui_delete_pane(eng, gui_plug->window, gui_plug->focus);
        engine_gui_node_focus(eng, gui_plug->focus);
        gui_plug->timestamp = now;
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (up & ENGINE_KEY_STATE_RELEASED) {
        engine_gui_node_unfocus(eng, gui_plug->focus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, UP);
        if (target != 0) {
          gui_plug->focus = target;
        }
        engine_gui_node_focus(eng, gui_plug->focus);
      } else if (down & ENGINE_KEY_STATE_RELEASED) {
        engine_gui_node_unfocus(eng, gui_plug->focus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, DOWN);
        if (target != 0) {
          gui_plug->focus = target;
        }
        engine_gui_node_focus(eng, gui_plug->focus);
      } else if (left & ENGINE_KEY_STATE_RELEASED) {
        engine_gui_node_unfocus(eng, gui_plug->focus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, LEFT);
        if (target != 0) {
          gui_plug->focus = target;
        }
        engine_gui_node_focus(eng, gui_plug->focus);
      } else if (right & ENGINE_KEY_STATE_RELEASED) {
        engine_gui_node_unfocus(eng, gui_plug->focus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, RIGHT);
        if (target != 0) {
          gui_plug->focus = target;
        }
        engine_gui_node_focus(eng, gui_plug->focus);
      }
    }
  }
}
