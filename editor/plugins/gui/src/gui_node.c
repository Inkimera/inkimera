#include "stdlib.h"

#include "gui_node.h"

const struct nk_color INK_BLACK = { 64, 64, 64, 255 };
const struct nk_color INK_BLACK_INK = { 13, 13, 13, 255 };
const struct nk_color INK_WHITE = { 242, 242, 242, 255 };
const struct nk_color INK_GREY = { 217, 217, 217, 255 };
const struct nk_color INK_DARK_GREY = { 129, 129, 129, 255 };
//const struct nk_color INK_LIGHT_BONE = { 235, 229, 217, 255 };
//const struct nk_color INK_BONE = { 130, 117, 99, 255 };
const struct nk_color INK_TRANSPARENT = { 0, 0, 0, 0 };

/* gui_hash */
unsigned long
gui_hash(
  const char *str
) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

/* gui_get_homedir */
int
gui_get_homedir(
  char *homedir
) {
#ifdef _WIN32
  return snprintf(homedir, MAX_PATH, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
  return snprintf(homedir, MAX_PATH, "%s", getenv("HOME"));
#endif
}

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
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

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
node_id_t
gui_new_pane(
  engine_t *eng,
  char *label,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

  gui_layout_t layout = { 0 };

  ecs_entity_t pane = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, pane, gui_node_type_t, { GUI_NODE_PANE });
  ecs_set_ptr(ecs_ctx, pane, gui_label_t, &label);
  ecs_set_ptr(ecs_ctx, pane, gui_layout_t, &layout);
  ecs_add_pair(ecs_ctx, pane, EcsChildOf, parent);
  return pane;
}

/* gui_new_vsplit */
node_id_t
gui_new_vsplit(
  engine_t *eng,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

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
node_id_t
gui_new_hsplit(
  engine_t *eng,
  node_id_t parent
) {
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);

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

/* gui_new_widget */
node_id_t
gui_new_widget(
  ecs_world_t *ecs_ctx,
  node_id_t parent,
  gui_widget_type_t widget_type
) {
  ecs_entity_t widget = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, widget, gui_node_type_t, { GUI_NODE_WIDGET });
  ecs_set(ecs_ctx, widget, gui_widget_type_t, { widget_type });
  ecs_set(ecs_ctx, widget, gui_state_t, { 0 });
  ecs_add_pair(ecs_ctx, widget, EcsChildOf, parent);
  return widget;
}

/* gui_project */
void
gui_project(
  ecs_world_t *ecs_ctx,
  node_id_t window,
  project_t project
) {
  ecs_entity_t project_node = ecs_new_id(ecs_ctx);
  ecs_add_pair(ecs_ctx, window, ProjectOf, project_node);
  ecs_add_pair(ecs_ctx, project_node, ProjectFor, window);
  if (project.num_scenes == 0) {
    project.scenes[0] = (project_scene_t) {
      .name = "untitled"
    };
    project.num_scenes = 1;
    ecs_entity_t scene = ecs_new_id(ecs_ctx);
    ecs_add(ecs_ctx, scene, ThreeDScene);
    ecs_set(ecs_ctx, scene, threed_scene_t, {
      .scene_id = gui_hash("untitled"),
      .name = "untitled"
    });
    ecs_add_pair(ecs_ctx, scene, EcsChildOf, project_node);
  } else {
    for (int i = 0; i < project.num_scenes; i++) {
      project_scene_t p_scene = project.scenes[i];
      threed_scene_t s = {
        .scene_id = gui_hash(p_scene.name),
        .name = { 0 }
      };
      strcpy(s.name, p_scene.name);

      ecs_entity_t scene_node = ecs_new_id(ecs_ctx);
      ecs_add(ecs_ctx, scene_node, ThreeDScene);
      ecs_set_ptr(ecs_ctx, scene_node, threed_scene_t, &s);
      ecs_add_pair(ecs_ctx, scene_node, EcsChildOf, project_node);
    }
  }
  ecs_set_ptr(ecs_ctx, project_node, project_t, &project);
}
