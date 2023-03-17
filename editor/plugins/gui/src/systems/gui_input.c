#include "gui.h"
#include "gui_node.h"
#include "systems/gui_input.h"

/*
 * GUI Node Tree
 */

/* gui_get_children */
static void
gui_delete_children(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  ecs_iter_t it = ecs_children(ecs_ctx, node);
  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      ecs_entity_t child = it.entities[i];
      node_id_t camera_node = ecs_get_target(ecs_ctx, child, ActiveCameraFor, 0);
      if (camera_node) {
        ecs_delete(ecs_ctx, camera_node);
      }
      ecs_delete(ecs_ctx, child);
    }
  }
}
/* gui_get_children */
static int
gui_get_children(
  engine_t *eng,
  node_id_t node,
  node_id_t *children
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  node_id_t chldrn[2] = { 0 };
  int num_children = ink_ecs_get_children_of(ecs_ctx, node, chldrn, 2);
  const gui_anchor_t *a_anchor = ecs_get(ecs_ctx, chldrn[0], gui_anchor_t);
  if (*a_anchor == GUI_ANCHOR_A) {
    children[0] = chldrn[0];
    children[1] = chldrn[1];
  } else {
    children[0] = chldrn[1];
    children[1] = chldrn[0];
  }
  return num_children;
}

/* gui_search_for_split */
static node_id_t
gui_search_for_split(
  engine_t *eng,
  node_id_t leaf,
  gui_node_type_t type
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  node_id_t parent = ecs_get_target(ecs_ctx, leaf, EcsChildOf, 0);
  if (parent == 0) {
    return 0;
  } else {
    const gui_node_type_t *parent_type = ecs_get(ecs_ctx, parent, gui_node_type_t);
    if (*parent_type == type) {
      return parent;
    } else {
      return gui_search_for_split(eng, parent, type);
    }
  }
}

/* gui_set_xoff_yoff */
static void
gui_set_xoff_yoff(
  engine_t *eng,
  node_id_t node,
  int xoff,
  int yoff
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

  gui_layout_t layout = {
    .xoff = xoff,
    .yoff = yoff
  };
  ecs_set_ptr(ecs_ctx, node, gui_layout_t, &layout);
}

/* gui_search_next_pane */
static node_id_t
gui_search_next_pane(
  engine_t *eng,
  node_id_t node,
  gui_search_direction_t direction
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  const gui_node_type_t *type = ecs_get(ecs_ctx, node, gui_node_type_t);
  if (*type == GUI_NODE_PANE) {
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
static node_id_t
gui_reverse_search_next_pane(
  engine_t *eng,
  node_id_t node,
  gui_search_direction_t direction
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  node_id_t parent = ecs_get_target(ecs_ctx, node, EcsChildOf, 0);
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
  const gui_layout_t *slayout = ecs_get(ecs_ctx, sibling, gui_layout_t);
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
static void
gui_resize_pane_vertical(
  engine_t *eng,
  node_id_t node,
  float delta
) {
  node_id_t target = gui_search_for_split(eng, node, GUI_NODE_VSPLIT);
  if (target == 0) {
    return;
  }
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  const gui_size_t *old_size = ecs_get(ecs_ctx, target, gui_size_t);
  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .value = {
      .dynamic = {
        .width = old_size->value.dynamic.width + delta,
        .height = old_size->value.dynamic.height
      }
    }
  };
  if (size.value.dynamic.width < 0.0) {
    size.value.dynamic.width = 0.0;
  }
  if (size.value.dynamic.width > 1.0) {
    size.value.dynamic.width = 1.0;
  }
  ecs_set_ptr(ecs_ctx, target, gui_size_t, &size);
}

/* gui_resize_pane_horizontal */
static void
gui_resize_pane_horizontal(
  engine_t *eng,
  node_id_t node,
  float delta
) {
  node_id_t target = gui_search_for_split(eng, node, GUI_NODE_HSPLIT);
  if (target == 0) {
    return;
  }
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  const gui_size_t *old_size = ecs_get(ecs_ctx, target, gui_size_t);
  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .value = {
      .dynamic = {
        .width = old_size->value.dynamic.width,
        .height = old_size->value.dynamic.height + delta
      }
    }
  };
  if (size.value.dynamic.height < 0.0) {
    size.value.dynamic.height = 0.0;
  }
  if (size.value.dynamic.height > 1.0) {
    size.value.dynamic.height = 1.0;
  }
  ecs_set_ptr(ecs_ctx, target, gui_size_t, &size);
}

/* gui_split_pane_vertical */
static node_id_t
gui_split_pane_vertical(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

  node_id_t parent = ecs_get_target(ecs_ctx, node, EcsChildOf, 0);
  node_id_t vsplit = gui_new_vsplit(eng, parent);
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
  ecs_set_ptr(ecs_ctx, vsplit, gui_anchor_t, anchor);
  ecs_set_ptr(ecs_ctx, vsplit, gui_layout_t, layout);

  node_id_t pane1 = node;
  ecs_add_pair(ecs_ctx, pane1, EcsChildOf, vsplit);
  ecs_set(ecs_ctx, pane1, gui_anchor_t, { GUI_ANCHOR_A });
  gui_set_xoff_yoff(eng, pane1, -1, 0);

  node_id_t pane2 = gui_new_pane(eng, "", vsplit);
  ecs_set(ecs_ctx, pane2, gui_anchor_t, { GUI_ANCHOR_B });
  gui_set_xoff_yoff(eng, pane2, 1, 0);
  gui_new_widget(ecs_ctx, pane2, GUI_WIDGET_SCENE_GRAPH);
  return pane2;
}

/* gui_split_pane_horizontal */
static node_id_t
gui_split_pane_horizontal(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

  node_id_t parent = ecs_get_target(ecs_ctx, node, EcsChildOf, 0);
  node_id_t hsplit = gui_new_hsplit(eng, parent);
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, node, gui_anchor_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, node, gui_layout_t);
  ecs_set_ptr(ecs_ctx, hsplit, gui_anchor_t, anchor);
  ecs_set_ptr(ecs_ctx, hsplit, gui_layout_t, layout);

  node_id_t pane1 = node;
  ecs_add_pair(ecs_ctx, pane1, EcsChildOf, hsplit);
  ecs_set(ecs_ctx, pane1, gui_anchor_t, { GUI_ANCHOR_A });
  gui_set_xoff_yoff(eng, pane1, 0, 1);

  node_id_t pane2 = gui_new_pane(eng, "", hsplit);
  ecs_set(ecs_ctx, pane2, gui_anchor_t, { GUI_ANCHOR_B });
  gui_set_xoff_yoff(eng, pane2, 0, -1);
  gui_new_widget(ecs_ctx, pane2, GUI_WIDGET_SCENE_GRAPH);
  return pane2;
}

/* gui_delete_pane */
static node_id_t
gui_delete_pane(
  engine_t *eng,
  node_id_t window,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

  node_id_t parent = ecs_get_target(ecs_ctx, node, EcsChildOf, 0);
  if (parent == 0 || parent == window) {
    return 0;
  }
  node_id_t grand_parent = ecs_get_target(ecs_ctx, parent, EcsChildOf, 0);
  ecs_delete(ecs_ctx, node);
  node_id_t children[1] = { 0 };
  ink_ecs_get_children_of(ecs_ctx, parent, children, 1);
  const gui_anchor_t *anchor = ecs_get(ecs_ctx, parent, gui_anchor_t);
  const gui_layout_t *layout = ecs_get(ecs_ctx, parent, gui_layout_t);
  node_id_t sibling = children[0];
  ecs_set_ptr(ecs_ctx, sibling, gui_anchor_t, anchor);
  ecs_set_ptr(ecs_ctx, sibling, gui_layout_t, layout);

  ecs_add_pair(ecs_ctx, sibling, EcsChildOf, grand_parent);
  ecs_delete(ecs_ctx, parent);

  node_id_t target = gui_search_next_pane(eng, sibling, UP);
  return target;
}

/* gui_plugin_input_system */
void
gui_plugin_input_system(
  ecs_iter_t *it
) {
  const plugin_t *plugs = ecs_field(it, plugin_t, 1);
  engine_t *eng = plugs[0].eng;
  gui_plugin_t *gui_plug = (gui_plugin_t*)plugs[0].plugin;

  ink_key_state_t escape = ink_key_state_get(INK_KEY_ESCAPE);
  ink_key_state_t ctrl = ink_key_state_get(INK_KEY_LEFT_CONTROL) | ink_key_state_get(INK_KEY_RIGHT_CONTROL);
  ink_key_state_t a = ink_key_state_get(INK_KEY_A);

  time_t now = time(NULL);
  if (strcmp(state_machine_get_state(&gui_plug->state), "idle") != 0 && now - gui_plug->timestamp > 2) {
    state_machine_set_state(&gui_plug->state, "idle");
    gui_plug->timestamp = now;
  }
  int changed_state = state_machine_process(&gui_plug->state, (int[3]) { INK_KEY_ESCAPE, INK_KEY_LEFT_CONTROL, INK_KEY_A }, (int[3]) { escape, ctrl, a }, 3);
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
    ecs_world_t *ecs_ctx = ink_ecs_context(eng);
    char *current_state = state_machine_get_state(&gui_plug->state);
    ink_key_state_t up = ink_key_state_get(INK_KEY_UP);
    ink_key_state_t down = ink_key_state_get(INK_KEY_DOWN);
    ink_key_state_t left = ink_key_state_get(INK_KEY_LEFT);
    ink_key_state_t right = ink_key_state_get(INK_KEY_RIGHT);
    if (strcmp(current_state, "idle") == 0) {
    } else if (strcmp(current_state, "leader_key_held") == 0) {
      if (up & INK_KEY_STATE_RELEASED) {
        gui_resize_pane_horizontal(eng, gui_plug->focus, -0.05);
        gui_plug->timestamp = now;
      } else if (down & INK_KEY_STATE_RELEASED) {
        gui_resize_pane_horizontal(eng, gui_plug->focus, 0.05);
        gui_plug->timestamp = now;
      }
      if (left & INK_KEY_STATE_RELEASED) {
        gui_resize_pane_vertical(eng, gui_plug->focus, -0.05);
        gui_plug->timestamp = now;
      } else if (right & INK_KEY_STATE_RELEASED) {
        gui_resize_pane_vertical(eng, gui_plug->focus, 0.05);
        gui_plug->timestamp = now;
      }
    } else if (strcmp(current_state, "leader_key_released") == 0) {
      ink_key_state_t shift = ink_key_state_get(INK_KEY_LEFT_SHIFT)
        | ink_key_state_get(INK_KEY_RIGHT_SHIFT);
      ink_key_state_t five = ink_key_state_get(INK_KEY_FIVE);
      ink_key_state_t apostrophe = ink_key_state_get(INK_KEY_APOSTROPHE);
      ink_key_state_t x = ink_key_state_get(INK_KEY_X);
      ink_key_state_t s = ink_key_state_get(INK_KEY_S);
      ink_key_state_t g = ink_key_state_get(INK_KEY_G);
      ink_key_state_t t = ink_key_state_get(INK_KEY_T);
      if (shift & INK_KEY_STATE_DOWN
          && five & INK_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        gui_plug->focus = gui_split_pane_vertical(eng, gui_plug->focus);
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (shift & INK_KEY_STATE_DOWN
                 && apostrophe & INK_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        gui_plug->focus = gui_split_pane_horizontal(eng, gui_plug->focus);
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (x & INK_KEY_STATE_RELEASED) {
        node_id_t target = gui_delete_pane(eng, gui_plug->window, gui_plug->focus);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        gui_plug->timestamp = now;
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (up & INK_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, UP);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (down & INK_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, DOWN);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (left & INK_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, LEFT);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (right & INK_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, RIGHT);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (s & INK_KEY_STATE_RELEASED) {
        gui_delete_children(eng, gui_plug->focus);
        gui_new_widget(ecs_ctx, gui_plug->focus, GUI_WIDGET_SCENE);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (g & INK_KEY_STATE_RELEASED) {
        gui_delete_children(eng, gui_plug->focus);
        gui_new_widget(ecs_ctx, gui_plug->focus, GUI_WIDGET_SCENE_GRAPH);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (t & INK_KEY_STATE_RELEASED) {
        gui_delete_children(eng, gui_plug->focus);
        gui_new_widget(ecs_ctx, gui_plug->focus, GUI_WIDGET_TERM);
        state_machine_set_state(&gui_plug->state, "idle");
      }
    }
  }
}
