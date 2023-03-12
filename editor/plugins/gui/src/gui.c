#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "inkimera.h"
#include "gui.h"

static char *PANE_ITEMS[] = { "SCENE", "TERM", "PROPS", "NODES" };
static int NUM_PANE_ITEMS = 4;

/* GuiPlugin */
ECS_TAG_DECLARE(GuiPlugin);

/*
 * GUI_PLUGIN
 */
static node_id_t
gui_new_window(
  engine_t *eng,
  int x,
  int y,
  float height,
  float width,
  char *label
);

/* gui_new_pane */
static node_id_t
gui_new_pane(
  engine_t *eng,
  char *label,
  node_id_t parent
);

/* gui_new_vsplit */
static node_id_t
gui_new_vsplit(
  engine_t *eng,
  node_id_t parent
);

/* gui_new_hsplit */
static node_id_t
gui_new_hsplit(
  engine_t *eng,
  node_id_t parent
);

/* gui_new_button */
static node_id_t
gui_new_button(
  engine_t *eng,
  char *label,
  node_id_t parent
);

/* gui_new_textbox */
static node_id_t
gui_new_textbox(
  engine_t *eng,
  node_id_t parent
);

/* gui_new_dropdown */
static node_id_t
gui_new_dropdown(
  engine_t *eng,
  node_id_t parent,
  char **items,
  int num_items,
  int width,
  int height
);

/* gui_new_scene */
static node_id_t
gui_new_scene(
  ecs_world_t *ecs_ctx,
  node_id_t parent
);

void
gui_dropdown_handle(
  ecs_world_t *ecs_ctx,
  ecs_entity_t node,
  gui_state_t *state
) {
  printf("gui_button_handle Node(%lld)\n", (long long)node);
  printf("State Mode(%i) Active(%i)\n", state->mode, state->active);
  if (state->active == 0) {
    printf("Selected: ");
    node_id_t parent = ecs_get_target(ecs_ctx, node, EcsChildOf, 0);
    node_id_t chldrn[2] = { 0 };
    int num_children = engine_ecs_get_children_of(ecs_ctx, parent, chldrn, 2);
    node_id_t target;
    const gui_node_type_t *type = ecs_get(ecs_ctx, chldrn[0], gui_node_type_t);
    if (*type == GUI_NODE_CONTAINER) {
      target = chldrn[0];
    } else {
      target = chldrn[1];
    }
    if (target != 0) {
      ecs_iter_t it = ecs_children(ecs_ctx, target);
      while (ecs_children_next(&it)) {
        for (int i = 0; i < it.count; i++) {
          ecs_entity_t child = it.entities[i];
          ecs_delete(ecs_ctx, child);
        }
      }
    }
    switch (state->mode) {
    case 0:
      printf("SCENE MODE\n");
      gui_new_scene(ecs_ctx, target);
      break;
    case 1:
      printf("TERM MODE\n");
      break;
    case 2:
      printf("PROPS MODE\n");
      break;
    case 3:
      printf("NODES MODE\n");
      ecs_entity_t textbox = ecs_new_id(ecs_ctx);
      ecs_set(ecs_ctx, textbox, gui_node_type_t, { GUI_NODE_SCROLL_VIEW });
      ecs_add_pair(ecs_ctx, textbox, EcsChildOf, target);
      break;
    }
  }
}

/* gui_new_window */
static node_id_t
gui_new_window(
  engine_t *eng,
  int x,
  int y,
  float height,
  float width,
  char *label
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  gui_position_t position = {
    .type = GuiDimensionTypeDynamic,
    .value = { .dynamic = { .x = x, .y = y } }
  };
  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .value = { .dynamic = { .width = width, .height = height } }
  };
  gui_layout_t layout = { 0 };

  ecs_entity_t window = ecs_new_id(ecs_ctx);
  ecs_add(ecs_ctx, window, GuiRoot);
  ecs_set(ecs_ctx, window, gui_node_type_t, { GUI_NODE_WINDOW });
  ecs_set_ptr(ecs_ctx, window, gui_label_t, &label);
  ecs_set_ptr(ecs_ctx, window, gui_position_t, &position);
  ecs_set_ptr(ecs_ctx, window, gui_size_t, &size);
  ecs_set_ptr(ecs_ctx, window, gui_layout_t, &layout);
  return window;
}

/* gui_new_pane */
static node_id_t
gui_new_pane(
  engine_t *eng,
  char *label,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  gui_layout_t layout = { 0 };

  ecs_entity_t pane = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, pane, gui_node_type_t, { GUI_NODE_PANE });
  ecs_set_ptr(ecs_ctx, pane, gui_label_t, &label);
  ecs_set_ptr(ecs_ctx, pane, gui_layout_t, &layout);
  ecs_add_pair(ecs_ctx, pane, EcsChildOf, parent);
  return pane;
}

/* gui_new_vsplit */
static node_id_t
gui_new_vsplit(
  engine_t *eng,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .value = { .dynamic = { .width = 0.5 } }
  };
  gui_layout_t layout = { 0 };

  ecs_entity_t split = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, split, gui_node_type_t, { GUI_NODE_VSPLIT });
  ecs_set_ptr(ecs_ctx, split, gui_size_t, &size);
  ecs_set_ptr(ecs_ctx, split, gui_layout_t, &layout);
  ecs_add_pair(ecs_ctx, split, EcsChildOf, parent);
  return split;
}

/* gui_new_hsplit */
static node_id_t
gui_new_hsplit(
  engine_t *eng,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  gui_size_t size = {
    .type = GuiDimensionTypeDynamic,
    .value = { .dynamic = { .height = 0.5 } }
  };
  gui_layout_t layout = { 0 };

  ecs_entity_t split = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, split, gui_node_type_t, { GUI_NODE_HSPLIT });
  ecs_set_ptr(ecs_ctx, split, gui_size_t, &size);
  ecs_set_ptr(ecs_ctx, split, gui_layout_t, &layout);
  ecs_add_pair(ecs_ctx, split, EcsChildOf, parent);
  return split;
}

/* gui_new_row */
/*
   static node_id_t
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
    .value = { .dynamic = { .height = height } }
   };
   engine_gui_node_set_size(eng, row, row_size);
   gui_layout_t row_layout = {
    .columns = columns,
   };
   engine_gui_node_set_layout(eng, row, row_layout);
   engine_ecs_parent(eng, row, parent);
   return row;
   }
 */

/* gui_new_column */
/*
   static node_id_t
   gui_new_column(
   engine_t *eng,
   float width,
   node_id_t parent
   ) {
   node_id_t column = engine_gui_node_create(eng);
   engine_gui_node_set_type(eng, column, GUI_NODE_COLUMN);
   gui_size_t column_size = {
    .type = GuiDimensionTypeDynamic,
    .value = { .dynamic = { .width = width } }
   };
   engine_gui_node_set_size(eng, column, column_size);
   engine_ecs_parent(eng, column, parent);
   return column;
   }
 */

/* gui_new_button */
static node_id_t
gui_new_button(
  engine_t *eng,
  char *label,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  ecs_entity_t button = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, button, gui_node_type_t, { GUI_NODE_BUTTON });
  ecs_set_ptr(ecs_ctx, button, gui_label_t, &label);
  ecs_add_pair(ecs_ctx, button, EcsChildOf, parent);
  return button;
}

/* gui_new_textbox */
static node_id_t
gui_new_textbox(
  engine_t *eng,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  ecs_entity_t textbox = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, textbox, gui_node_type_t, { GUI_NODE_TEXTBOX });
  ecs_add_pair(ecs_ctx, textbox, EcsChildOf, parent);
  return textbox;
}

/* gui_new_dropdown */
static node_id_t
gui_new_dropdown(
  engine_t *eng,
  node_id_t parent,
  char **items,
  int num_items,
  int width,
  int height
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  gui_size_t size = {
    .type = GuiDimensionTypeFixed,
    .value = { .fixed = { .width = width, .height = height } }
  };
  gui_state_t state = { 0 };
  gui_event_handle_t handle = {
    .handle = &gui_dropdown_handle
  };
  gui_list_t list = {
    .items = items,
    .num_items = num_items
  };

  ecs_entity_t dropdown = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, dropdown, gui_node_type_t, { GUI_NODE_DROPDOWN });
  ecs_set_ptr(ecs_ctx, dropdown, gui_list_t, &list);
  ecs_set_ptr(ecs_ctx, dropdown, gui_size_t, &size);
  ecs_set_ptr(ecs_ctx, dropdown, gui_state_t, &state);
  ecs_set_ptr(ecs_ctx, dropdown, gui_event_handle_t, &handle);
  ecs_add_pair(ecs_ctx, dropdown, EcsChildOf, parent);
  return dropdown;
}

/* gui_new_scene */
static node_id_t
gui_new_scene(
  ecs_world_t *ecs_ctx,
  node_id_t parent
) {
  //ecs_world_t *ecs_ctx = engine_ecs_context(eng);
  // Scene
  ecs_entity_t scene = ecs_new_id(ecs_ctx);
  ecs_add(ecs_ctx, scene, ThreeDNodeActive);
  ecs_set(ecs_ctx, scene, threed_scene_t, { .scene_id = 0 });
  ecs_set(ecs_ctx, scene, gui_node_type_t, { GUI_NODE_SCENE });
  ecs_add_pair(ecs_ctx, scene, EcsChildOf, parent);

  // TEST
  ecs_entity_t node = ecs_new_id(ecs_ctx);
  ecs_add(ecs_ctx, node, ThreeDNodeActive);
  ecs_set(ecs_ctx, node, threed_position_t, { .y = 0.0, .z = 10.0 });
  ecs_set(ecs_ctx, node, threed_rotation_t, { .x = 15.0, .y = 0.0 });
  ecs_add_pair(ecs_ctx, node, EcsChildOf, scene);


  // Camera
  ecs_entity_t camera = ecs_new_id(ecs_ctx);
  threed_render_texture_t render_texture = {
    .texture = LoadRenderTexture(1280, 720)
  };
  threed_camera_t camera_props = {
    .fov = 90.0,
    .up = { .y = 1.0 },
    .target = { .z = 1.0 }
  };
  ecs_add(ecs_ctx, camera, ThreeDNodeActive);
  ecs_set_ptr(ecs_ctx, camera, threed_render_texture_t, &render_texture);
  ecs_set_ptr(ecs_ctx, camera, threed_camera_t, &camera_props);
  ecs_set(ecs_ctx, camera, threed_position_t, { .y = 0.0, .z = 0.0 });
  ecs_set(ecs_ctx, camera, threed_rotation_t, { .y = 0.0 });
  ecs_add_pair(ecs_ctx, camera, EcsChildOf, node);

  ecs_add_pair(ecs_ctx, camera, CameraOf, scene);
  ecs_add_pair(ecs_ctx, scene, ActiveCameraFor, camera);
  return scene;
}

/*
 * GUI Node Tree
 */

/* gui_search_direction_t */
typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} gui_search_direction_t;

/* gui_get_children */
static int
gui_get_children(
  engine_t *eng,
  node_id_t node,
  node_id_t *children
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
  node_id_t chldrn[2] = { 0 };
  int num_children = engine_ecs_get_children_of(ecs_ctx, node, chldrn, 2);
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

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
  gui_new_dropdown(eng, pane2, PANE_ITEMS, NUM_PANE_ITEMS, 135, 30);
  ecs_entity_t container = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, container, gui_node_type_t, { GUI_NODE_CONTAINER });
  ecs_add_pair(ecs_ctx, container, EcsChildOf, pane2);
  gui_new_scene(ecs_ctx, container);
  return pane2;
}

/* gui_split_pane_horizontal */
static node_id_t
gui_split_pane_horizontal(
  engine_t *eng,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

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
  gui_new_dropdown(eng, pane2, PANE_ITEMS, NUM_PANE_ITEMS, 135, 30);
  ecs_entity_t container = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, container, gui_node_type_t, { GUI_NODE_CONTAINER });
  ecs_add_pair(ecs_ctx, container, EcsChildOf, pane2);
  gui_new_scene(ecs_ctx, container);
  return pane2;
}

/* gui_delete_pane */
static node_id_t
gui_delete_pane(
  engine_t *eng,
  node_id_t window,
  node_id_t node
) {
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);

  node_id_t parent = ecs_get_target(ecs_ctx, node, EcsChildOf, 0);
  if (parent == 0 || parent == window) {
    return 0;
  }
  node_id_t grand_parent = ecs_get_target(ecs_ctx, parent, EcsChildOf, 0);
  ecs_delete(ecs_ctx, node);
  node_id_t children[1] = { 0 };
  engine_ecs_get_children_of(ecs_ctx, parent, children, 1);
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

/*
 * GUI CORE
 */

/* gui_plugin_init */
node_id_t
gui_plugin_init(
  engine_t *eng
) {
  printf("INIT gui plugin\n");
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
  ECS_TAG_DEFINE(ecs_ctx, GuiPlugin);
  ECS_SYSTEM(ecs_ctx, gui_plugin_system, InkOnInput, plugin_t, GuiPlugin);

  ecs_entity_t plugin = ecs_new_id(ecs_ctx);
  gui_plugin_t *plug = malloc(sizeof(gui_plugin_t));
  ecs_add(ecs_ctx, plugin, GuiPlugin);
  ecs_set(ecs_ctx, plugin, plugin_t, {
    .eng = eng,
    .plugin = plug,
    .load = &gui_plugin_load,
    .unload = &gui_plugin_unload
  });
  return plugin;
}

/* gui_plugin_deinit */
int
gui_plugin_deinit(
  gui_plugin_t *plug
) {
  printf("DEINIT gui plugin\n");
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
  ecs_world_t *ecs_ctx = engine_ecs_context(eng);
  node_id_t window = gui_new_window(eng, 0, 0, 1.0, 1.0, "Inkimera Editor");
  node_id_t pane = gui_new_pane(eng, "", window);
  ecs_set(ecs_ctx, pane, gui_anchor_t, { GUI_ANCHOR_ROOT });
  ecs_add(ecs_ctx, pane, GuiFocus);

  gui_new_dropdown(eng, pane, PANE_ITEMS, NUM_PANE_ITEMS, 135, 30);
  ecs_entity_t container = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, container, gui_node_type_t, { GUI_NODE_CONTAINER });
  ecs_add_pair(ecs_ctx, container, EcsChildOf, pane);
  gui_new_scene(ecs_ctx, container);

  gui_plug->window = window;
  gui_plug->focus = pane;
  gui_plug->timestamp = time(NULL);

  // Setup
  state_machine_state_t *idle_state = state_machine_add_state(&gui_plug->state, "idle");
  state_machine_state_t *leader_key_held_state = state_machine_add_state(&gui_plug->state, "leader_key_held");
  state_machine_state_t *leader_key_released_state = state_machine_add_state(&gui_plug->state, "leader_key_released");
  //state_machine_state_t *command_state = state_machine_add_state(&gui_plug->state, "command");

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
  (void)eng;
  gui_plugin_t *gui_plug = (gui_plugin_t*)plug;
  return gui_plugin_deinit(gui_plug);
}

/* gui_plugin_system */
void
gui_plugin_system(
  ecs_iter_t *it
) {
  const plugin_t *plugs = ecs_field(it, plugin_t, 1);
  engine_t *eng = plugs[0].eng;
  gui_plugin_t *gui_plug = (gui_plugin_t*)plugs[0].plugin;

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
    ecs_world_t *ecs_ctx = engine_ecs_context(eng);
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
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        gui_plug->focus = gui_split_pane_vertical(eng, gui_plug->focus);
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (shift & ENGINE_KEY_STATE_DOWN
                 && apostrophe & ENGINE_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        gui_plug->focus = gui_split_pane_horizontal(eng, gui_plug->focus);
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (x & ENGINE_KEY_STATE_RELEASED) {
        node_id_t target = gui_delete_pane(eng, gui_plug->window, gui_plug->focus);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
        gui_plug->timestamp = now;
        state_machine_set_state(&gui_plug->state, "idle");
      } else if (up & ENGINE_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, UP);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
      } else if (down & ENGINE_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, DOWN);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
      } else if (left & ENGINE_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, LEFT);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
      } else if (right & ENGINE_KEY_STATE_RELEASED) {
        ecs_remove(ecs_ctx, gui_plug->focus, GuiFocus);
        node_id_t target = gui_reverse_search_next_pane(eng, gui_plug->focus, RIGHT);
        if (target != 0) {
          gui_plug->focus = target;
        }
        ecs_add(ecs_ctx, gui_plug->focus, GuiFocus);
      }
    }
  }
}
