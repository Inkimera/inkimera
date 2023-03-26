#include "gui_node.h"
#include "widgets/gui_menu_header_widget.h"
#include "widgets/gui_menu/gui_new_project_widget.h"
#include "widgets/gui_menu/gui_open_project_widget.h"

/* gui_new_project_popup_widget */
bool
gui_new_project_popup_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container
) {
  bool close = false;
  static bool show_file_browser = false;
  int popup_width = 600;
  int popup_height = 400;
  if (!show_file_browser) {
    popup_height = 128 + 16 + 34;
  }
  int popup_x = GetRenderWidth() / 2 - popup_width / 2;
  int popup_y = GetRenderHeight() / 2 - popup_height / 2;
  struct nk_rect popup_rect = nk_rect(popup_x, popup_y, popup_width, popup_height);
  if (nk_popup_begin(nk_ctx, NK_POPUP_STATIC, "NewProjectPopup", NK_WINDOW_NO_SCROLLBAR, popup_rect)) {
    close = gui_new_project_widget(ecs_ctx, nk_ctx, project_node, project, (Rectangle) { 0, 0, popup_width, popup_height }, &show_file_browser);
    nk_popup_end(nk_ctx);
  }
  return close;
}

/* gui_open_project_popup_widget */
bool
gui_open_project_popup_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container
) {
  bool close = false;
  static bool show_file_browser = false;
  int popup_width = 600;
  int popup_height = 400;
  if (!show_file_browser) {
    popup_height = 96 + 34;
  }
  int popup_x = GetRenderWidth() / 2 - popup_width / 2;
  int popup_y = GetRenderHeight() / 2 - popup_height / 2;
  struct nk_rect popup_rect = nk_rect(popup_x, popup_y, popup_width, popup_height);
  if (nk_popup_begin(nk_ctx, NK_POPUP_STATIC, "OpenProjectPopup", NK_WINDOW_NO_SCROLLBAR, popup_rect)) {
    close = gui_open_project_widget(ecs_ctx, nk_ctx, project_node, project, (Rectangle) { 0, 0, popup_width, popup_height }, &show_file_browser);
    nk_popup_end(nk_ctx);
  }
  return close;
}

/* gui_header_widget */
void
gui_header_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  ecs_entity_t project_node,
  const project_t *project,
  Rectangle container
) {
  nk_style_push_color(nk_ctx, &nk_ctx->style.window.background, INK_BLACK_INK);
  nk_style_push_style_item(nk_ctx, &nk_ctx->style.window.fixed_background, nk_style_item_color(INK_BLACK_INK));
  static bool new_project_window = false;
  static bool open_project_window = false;
  static bool new_scene_window = false;
  if (nk_begin(nk_ctx, "InkMenuBar", nk_rect(container.x, container.y, container.width, container.height), NK_WINDOW_NO_SCROLLBAR)) {
    static enum nk_collapse_states state;
    nk_layout_space_begin(nk_ctx, NK_STATIC, 64, 2);
    nk_layout_space_push(nk_ctx, nk_rect(10, 16, 100, 32));
    nk_menubar_begin(nk_ctx);
    if (nk_menu_begin_label(nk_ctx, "Project", NK_LEFT, (struct nk_vec2) { 100.0, 100.0 })) {
      nk_layout_row_static(nk_ctx, 0.0, 100.0, 1);
      if (nk_menu_item_label(nk_ctx, "Save", NK_TEXT_LEFT)) {
        project_save(project);
      }
      if (nk_menu_item_label(nk_ctx, "New", NK_TEXT_LEFT)) {
        new_project_window = true;
      }
      if (nk_menu_item_label(nk_ctx, "Open", NK_TEXT_LEFT)) {
        open_project_window = true;
      }
      if (nk_menu_item_label(nk_ctx, "Exit", NK_TEXT_LEFT)) {
        ecs_singleton_set(ecs_ctx, engine_shutdown_t, { true });
      }
      nk_menu_end(nk_ctx);
    }
    nk_layout_space_push(nk_ctx, nk_rect(110, 16, 100, 32));
    if (nk_menu_begin_label(nk_ctx, "Scene", NK_LEFT, (struct nk_vec2) { 100.0, 100.0 })) {
      nk_layout_row_static(nk_ctx, 0.0, 100.0, 1);
      if (nk_menu_item_label(nk_ctx, "New", NK_TEXT_LEFT)) {
        char name[64];
        snprintf(name, 64, "undefined%i", project->num_scenes);
        threed_scene_t scene = {
          .scene_id = gui_hash(name),
          .name = { 0 }
        };
        strcpy(scene.name, name);

        ecs_entity_t scene_node = ecs_new_id(ecs_ctx);
        ecs_add(ecs_ctx, scene_node, ThreeDScene);
        ecs_set_ptr(ecs_ctx, scene_node, threed_scene_t, &scene);
        ecs_add_pair(ecs_ctx, scene_node, EcsChildOf, project_node);

        project_t p = *project;
        project_scene_t p_scene = { 0 };
        strcpy(p_scene.name, name);
        p.scenes[p.num_scenes] = p_scene;
        p.num_scenes++;
        ecs_set_ptr(ecs_ctx, project_node, project_t, &p);
      }
      nk_menu_end(nk_ctx);
    }
    nk_menubar_end(nk_ctx);
    nk_layout_space_push(nk_ctx, nk_rect(container.width / 2 - 100, 16, 200, 32));
    nk_style_push_color(nk_ctx, &nk_ctx->style.text.color, INK_WHITE);
    nk_label(nk_ctx, project->name, NK_TEXT_CENTERED);
    nk_style_pop_color(nk_ctx);
    nk_layout_space_end(nk_ctx);
  }
  if (new_project_window) {
    new_project_window = !gui_new_project_popup_widget(ecs_ctx, nk_ctx, project_node, project, container);
  }
  if (open_project_window) {
    open_project_window = !gui_open_project_popup_widget(ecs_ctx, nk_ctx, project_node, project, container);
  }
  nk_end(nk_ctx);
  nk_style_pop_color(nk_ctx);
  nk_style_pop_style_item(nk_ctx);
}
