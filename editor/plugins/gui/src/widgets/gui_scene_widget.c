#include "gui_node.h"
#include "widgets/gui_scene_widget.h"

/* gui_file_browser_widget */
static void
gui_file_browser_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  char *dir
) {
  nk_layout_row_dynamic(nk_ctx, 32, 1);
  static FilePathList paths = { 0 };
  static unsigned long last_hash = 0;
  static int selected_idx = -1;

  unsigned long hash = gui_hash(dir);
  if (last_hash != hash) {
    printf("DIR %s\n", dir);
    UnloadDirectoryFiles(paths);
    paths = LoadDirectoryFiles(dir);
    last_hash = hash;
  }
  for (int i = 0; i < paths.count; i++) {
    //if (!IsPathFile(paths.paths[i])) {
    nk_bool selected = false;
    if (selected_idx == i) {
      selected = true;
    }
    if (nk_selectable_label(nk_ctx, paths.paths[i], NK_TEXT_ALIGN_LEFT, &selected)) {
      if (selected_idx == i) {
        //strcpy(dir, paths.paths[i]);
      } else {
        selected_idx = i;
      }
    }
    //}
  }
  if (paths.count == 0) {
    nk_label(nk_ctx, "[empty]", NK_TEXT_ALIGN_CENTERED);
  }
}

/* gui_scene_widget_save_scene */
static bool
gui_scene_widget_save_scene(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container,
  ecs_entity_t widget,
  ecs_entity_t scene
) {
  bool show = true;
  static bool show_file_browser = false;
  int popup_width = 600;
  int popup_height = 400;
  if (!show_file_browser) {
    popup_height = 32 + 32 + 32 + 32 + 34;
  }
  int popup_x = container.width / 2 - popup_width / 2;
  int popup_y = container.height / 2 - popup_height / 2;
  struct nk_rect popup_rect = nk_rect(popup_x, popup_y, popup_width, popup_height);
  nk_style_push_color(nk_ctx, &nk_ctx->style.window.background, INK_BLACK_INK);
  nk_style_push_style_item(nk_ctx, &nk_ctx->style.window.fixed_background, nk_style_item_color(INK_BLACK_INK));
  if (nk_popup_begin(nk_ctx, NK_POPUP_STATIC, "SaveScenePopup", NK_WINDOW_NO_SCROLLBAR, popup_rect)) {
    nk_layout_row_dynamic(nk_ctx, 16, 1);
    nk_label(nk_ctx, "Create New Scene", NK_TEXT_ALIGN_CENTERED);
    nk_label(nk_ctx, "Scene Name:", NK_TEXT_LEFT);
    // Scene Path
    nk_layout_row_dynamic(nk_ctx, 32, 1);
    static char name[128] = { 0 };
    static int name_len = 0;
    nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, name, &name_len, 128, nk_filter_default);
    // Scene Path
    nk_layout_row_dynamic(nk_ctx, 32, 1);
    nk_label(nk_ctx, "Project Path:", NK_TEXT_LEFT);
    nk_layout_row_begin(nk_ctx, NK_DYNAMIC, 32, 2);
    static bool edited = false;
    static char path[MAX_PATH] = { 0 };
    static int path_len = 0;
    if (!edited) {
      const char *project_path = GetPrevDirectoryPath(project->path);
      path_len = snprintf(path, MAX_PATH, "%s", project_path);
      edited = true;
    } else {
      path_len = strlen(path);
    }
    nk_layout_row_push(nk_ctx, 0.75);
    nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, path, &path_len, MAX_PATH, nk_filter_default);
    for (int i = path_len; i < MAX_PATH; i++) {
      path[i] = 0;
    }
    nk_layout_row_push(nk_ctx, 0.25);
    if (nk_button_label(nk_ctx, "Browse")) {
      show_file_browser = !show_file_browser;
    }
    int y = popup_height - 32 - 32 - 32 - 32 - 34;
    if (show_file_browser) {
      nk_layout_row_dynamic(nk_ctx, y, 1);
      if (nk_group_begin(nk_ctx, "NewSceneFileBrowser", NK_WINDOW_SCROLL_AUTO_HIDE)) {
        gui_file_browser_widget(ecs_ctx, nk_ctx, container, path);
        nk_group_end(nk_ctx);
        y = 0;
      }
    }
    // Buttons
    nk_layout_space_begin(nk_ctx, NK_STATIC, 0, 2);
    nk_layout_space_push(nk_ctx, nk_rect(0, y, popup_width / 2, 32));
    if (nk_button_label(nk_ctx, "Cancel")) {
      show = false;
      edited = false;
      nk_popup_close(nk_ctx);
      memset(name, 0, name_len);
      name_len = 0;
    }
    nk_layout_space_push(nk_ctx, nk_rect(popup_width / 2, y, popup_width / 2, 32));
    if (nk_button_label(nk_ctx, "Create")) {
      show = false;
      edited = false;
      nk_popup_close(nk_ctx);
      const threed_scene_t *scene_data = ecs_get(ecs_ctx, scene, threed_scene_t);
      unsigned long hash = gui_hash(scene_data->name);
      threed_scene_t s = {
        .scene_id = gui_hash(name),
        .name = { 0 }
      };
      strcpy(s.name, name);

      ecs_set_ptr(ecs_ctx, scene, threed_scene_t, &s);

      project_t p = *project;
      project_scene_t p_scene = { 0 };
      strcpy(p_scene.name, name);
      strcpy(p_scene.path, path);
      for (int i = 0; i < project->num_scenes; i++) {
        project_scene_t s = project->scenes[i];
        if (gui_hash(s.name) == hash) {
          p.scenes[i] = p_scene;
        }
      }
      ecs_set_ptr(ecs_ctx, project_node, project_t, &p);
      const gui_state_t *state = ecs_get(ecs_ctx, widget, gui_state_t);
      ecs_set(ecs_ctx, widget, gui_state_t, { .mode = state->mode, .target = gui_hash(p_scene.name) });
      memset(name, 0, name_len);
      name_len = 0;
      project_scene_save(project->path, &p_scene);
    }
    nk_popup_end(nk_ctx);
  }
  nk_style_pop_color(nk_ctx);
  nk_style_pop_style_item(nk_ctx);
  return show;
}

/* gui_scene_widget_add_camera */
static void
gui_scene_widget_add_camera(
  ecs_world_t *ecs_ctx,
  ecs_entity_t widget,
  ecs_entity_t scene,
  Rectangle container
) {
  // Camera
  ecs_entity_t camera = ecs_new_id(ecs_ctx);
  ecs_add(ecs_ctx, camera, ThreeDNodeActive);
  ecs_set(ecs_ctx, camera, threed_viewport_t, {
    .xoff = container.x,
    .yoff = container.y,
    .width = container.width,
    .height = container.height
  });
  ecs_set(ecs_ctx, camera, threed_camera_t, {
    .fov = 90.0,
    .distance = 1.0,
    .up = { .x = 0.0, .y = 1.0, .z = 0.0 },
    .right = { .x = 1.0, .y = 0.0, .z = 0.0 },
    .forward = { .x = 0.0, .y = 0.0, .z = 1.0 },
    .target = { .x = 0.0, .y = 0.0, .z = 0.0 }
  });
  ecs_set(ecs_ctx, camera, threed_render_texture_t, {
    .fbo = LoadRenderTexture(container.width, container.height),
    .texture = LoadRenderTexture(container.width, container.height)
  });
  ecs_set(ecs_ctx, camera, threed_position_t, { .x = 0.0, .y = 0.0, .z = -1.0 });
  ecs_set(ecs_ctx, camera, threed_rotation_t, { .x = 0.0, .y = 0.0, .z = 0.0 });
  ecs_set(ecs_ctx, camera, threed_velocity_t, { .x = 0.0, .y = 0.0, .z = 0.0 });
  ecs_set(ecs_ctx, camera, threed_spin_t, { .x = 0.0, .y = 0.0, .z = 0.0 });
  ecs_add_pair(ecs_ctx, camera, EcsChildOf, scene);
  ecs_add_pair(ecs_ctx, camera, CameraOf, scene);

  ecs_add_pair(ecs_ctx, widget, ActiveCameraFor, camera);
}

/* gui_scene_widget */
void
gui_scene_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container,
  ecs_entity_t widget,
  ecs_entity_t scene
) {
  const gui_state_t *state = ecs_get(ecs_ctx, widget, gui_state_t);
  node_id_t camera = ecs_get_target(ecs_ctx, widget, ActiveCameraFor, 0);
  int texture_border = 20;
  Rectangle texture = container;
  texture.x += texture_border / 2;
  texture.y += texture_border / 2;
  texture.height -= texture_border;
  texture.width -= texture_border;
  if (!camera ) {
    printf("Scene Node(%lld) has no camera\n", (long long)scene);
    return gui_scene_widget_add_camera(ecs_ctx, widget, scene, texture);
  }
  const threed_render_texture_t *render_texture = ecs_get(ecs_ctx, camera, threed_render_texture_t);
  if (!render_texture) {
    printf("Invalid camera\n");
    return;
  }
  Rectangle rect = container;
  if (texture.width != render_texture->texture.texture.width || texture.height != render_texture->texture.texture.height) {
    printf("Resize viewport\n");
    UnloadRenderTexture(render_texture->fbo);
    UnloadRenderTexture(render_texture->texture);

    ecs_set(ecs_ctx, camera, threed_viewport_t, {
      .xoff = texture.x,
      .yoff = texture.y,
      .width = texture.width,
      .height = texture.height
    });
    ecs_set(ecs_ctx, camera, threed_render_texture_t, {
      .fbo = LoadRenderTexture(texture.width, texture.height),
      .texture = LoadRenderTexture(texture.width, texture.height)
    });
  }
  const threed_scene_t *scene_data = ecs_get(ecs_ctx, scene, threed_scene_t);
  const char *items[10];
  for (int i = 0; i < 10; i++) {
    if (i > project->num_scenes) {
      break;
    }
    items[i] = project->scenes[i].name;
  }
  int mode = state->mode;
  float width = 200;
  float height = 40;
  Rectangle ddrect = {
    .x = texture.width / 2 - width / 2,
    .y = 0,
    .width = width,
    .height = height
  };
  char window_name[128] = { 0 };
  snprintf(window_name, 128, "GUI_WIDGET_SCENE(%lld)", (long long)widget);
  const gui_texture_t *gui_texture = ecs_singleton_get(ecs_ctx, gui_texture_t);
  static bool show_save_menu = false;
  if (nk_begin(nk_ctx, window_name, nk_rect(rect.x, rect.y, rect.width, rect.height), NK_WINDOW_NO_SCROLLBAR)) {
    if (show_save_menu) {
      show_save_menu = gui_scene_widget_save_scene(ecs_ctx, nk_ctx, project_node, project, container, widget, scene);
    }
    nk_layout_space_begin(nk_ctx, NK_STATIC, texture.height, 1);
    nk_layout_space_push(nk_ctx, nk_rect(texture_border / 2, texture_border / 2, texture.width, texture.height));
    if (nk_group_begin(nk_ctx, "Scene", NK_WINDOW_NO_SCROLLBAR)) {
      nk_layout_row_static(nk_ctx, texture.height, texture.width, 1);
      struct nk_image img = nk_image_id(render_texture->texture.texture.id);
      nk_image(nk_ctx, img);
      nk_group_end(nk_ctx);
    }
    nk_style_push_color(nk_ctx, &nk_ctx->style.window.group_border_color, INK_BLACK_INK);
    nk_layout_space_push(nk_ctx, nk_rect(ddrect.x, ddrect.y - texture.height + 14, ddrect.width, ddrect.height));
    if (nk_group_begin(nk_ctx, "SceneMenu", NK_WINDOW_NO_SCROLLBAR)) {
      nk_layout_row_dynamic(nk_ctx, ddrect.height, 2);
      mode = nk_combo(nk_ctx, items, project->num_scenes, state->mode, ddrect.height, nk_vec2(ddrect.width, ddrect.height * project->num_scenes));
      if (mode != state->mode) {
        printf("New target is %s\n", items[mode]);
        ecs_delete(ecs_ctx, camera);
        ecs_set(ecs_ctx, widget, gui_state_t, { .mode = mode, .target = gui_hash(items[mode]) });
      }
      if (nk_button_label(nk_ctx, "Save")) {
        unsigned long long hash = gui_hash(scene_data->name);
        for (int i = 0; i < project->num_scenes; i++) {
          project_scene_t p_scene = project->scenes[i];
          if (gui_hash(p_scene.name) == hash) {
            if (strcmp(p_scene.path, "")) {
              // SAVE
            } else {
              show_save_menu = true;
            }
          }
        }
      }
      nk_group_end(nk_ctx);
    }
    nk_style_pop_color(nk_ctx);
    nk_layout_space_end(nk_ctx);
  }
  nk_end(nk_ctx);
}
