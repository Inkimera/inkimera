#include "widgets/gui_menu/gui_open_project_widget.h"
#include "gui_node.h"

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
    UnloadDirectoryFiles(paths);
    paths = LoadDirectoryFiles(dir);
    last_hash = hash;
  }
  for (int i = 0; i < paths.count; i++) {
    if (strstr(paths.paths[i], "project.ink")) {
      nk_label(nk_ctx, "[ink project]", NK_TEXT_ALIGN_CENTERED);
      return;
    }
  }
  for (int i = 0; i < paths.count; i++) {
    if (!IsPathFile(paths.paths[i])) {
      nk_bool selected = false;
      if (selected_idx == i) {
        selected = true;
      }
      if (nk_selectable_label(nk_ctx, paths.paths[i], NK_TEXT_ALIGN_LEFT, &selected)) {
        if (selected_idx == i) {
          strcpy(dir, paths.paths[i]);
        } else {
          selected_idx = i;
        }
      }
    }
  }
  if (paths.count == 0) {
    nk_label(nk_ctx, "[empty]", NK_TEXT_ALIGN_CENTERED);
  }
}

/* gui_open_project_widget */
bool
gui_open_project_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container,
  bool *show_file_browser
) {
  bool close = false;
  char homedir[MAX_PATH] = { 0 };
  gui_get_homedir(homedir);
  static bool edited = false;
  static char path[MAX_PATH] = { 0 };
  static int path_len = 0;
  if (!edited) {
    path_len = snprintf(path, MAX_PATH, "%s", homedir);
    edited = true;
  } else {
    path_len = strlen(path);
  }
  char project_path[MAX_PATH_SIZE + MAX_NAME_SIZE];
  snprintf(project_path, MAX_PATH_SIZE + MAX_NAME_SIZE, "%s/project.ink", path);
  bool project_exists = FileExists(project_path);
  nk_layout_row_dynamic(nk_ctx, 16, 1);
  nk_label(nk_ctx, "Open Project", NK_TEXT_ALIGN_CENTERED);
  // Project Path
  nk_layout_row_begin(nk_ctx, NK_DYNAMIC, 32, 2);
  nk_layout_row_push(nk_ctx, 0.25);
  nk_label(nk_ctx, "Project Path:", NK_TEXT_LEFT);
  nk_layout_row_push(nk_ctx, 0.75);
  if (project_exists) {
    nk_label(nk_ctx, project_path, NK_TEXT_LEFT);
  } else {
    nk_label(nk_ctx, "", NK_TEXT_LEFT);
  }
  nk_layout_row_begin(nk_ctx, NK_DYNAMIC, 32, 2);
  nk_layout_row_push(nk_ctx, 0.75);
  nk_edit_string(nk_ctx, NK_EDIT_SIMPLE, path, &path_len, 128, nk_filter_default);
  for (int i = path_len; i < MAX_PATH; i++) {
    path[i] = 0;
  }
  nk_layout_row_push(nk_ctx, 0.25);
  if (nk_button_label(nk_ctx, "Browse")) {
    *show_file_browser = !*show_file_browser;
  }
  int y = container.height - 80 - 34;
  if (*show_file_browser) {
    nk_layout_row_dynamic(nk_ctx, y, 1);
    if (nk_group_begin(nk_ctx, "NewProjectFileBrowser", NK_WINDOW_SCROLL_AUTO_HIDE)) {
      gui_file_browser_widget(ecs_ctx, nk_ctx, container, path);
      nk_group_end(nk_ctx);
      y = 0;
    }
  }
  // Buttons
  nk_layout_space_begin(nk_ctx, NK_STATIC, 0, 2);
  nk_layout_space_push(nk_ctx, nk_rect(0, y, container.width / 2, 32));
  if (nk_button_label(nk_ctx, "Cancel")) {
    close = true;
    memset(path, 0, path_len);
    path_len = 0;
  }
  nk_layout_space_push(nk_ctx, nk_rect(container.width / 2, y, container.width / 2, 32));
  if (nk_button_label(nk_ctx, "Open")) {
    if (project_exists) {
      close = true;
      ecs_entity_t window = ecs_get_target(ecs_ctx, project_node, ProjectFor, 0);
      ecs_delete(ecs_ctx, project_node);

      project_t p = project_load(project_path);
      gui_project(ecs_ctx, window, p);
    }
  }
  nk_layout_space_end(nk_ctx);
  return close;
}
