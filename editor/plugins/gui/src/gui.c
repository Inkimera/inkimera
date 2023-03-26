#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "project.h"

#include "gui.h"
#include "gui_node.h"
#include "systems/gui_input.h"
#include "systems/gui_update.h"

/* GuiPlugin */
ECS_TAG_DECLARE(GuiPlugin);

/* gui_node_type_t */
ECS_COMPONENT_DECLARE(gui_node_type_t);

/* gui_node_type_t */
ECS_COMPONENT_DECLARE(gui_widget_type_t);

/* gui_anchor_t */
ECS_COMPONENT_DECLARE(gui_anchor_t);

/* gui_label_t */
ECS_COMPONENT_DECLARE(gui_label_t);

/* gui_list_t */
ECS_COMPONENT_DECLARE(gui_list_t);

/* gui_position_t */
ECS_COMPONENT_DECLARE(gui_position_t);

/* gui_size_t */
ECS_COMPONENT_DECLARE(gui_size_t);

/* gui_layout_t */
ECS_COMPONENT_DECLARE(gui_layout_t);

/* gui_state_t */
ECS_COMPONENT_DECLARE(gui_state_t);

/* gui_button_handle_t */
ECS_COMPONENT_DECLARE(gui_event_handle_t);

/*
 * GUI CORE
 */

/* gui_new_project */
void
gui_new_project(
  void
) {

}

/* gui_plugin_init */
node_id_t
gui_plugin_init(
  engine_t *eng
) {
  printf("INIT gui plugin\n");
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  ECS_TAG_DEFINE(ecs_ctx, GuiPlugin);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_node_type_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_widget_type_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_anchor_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_label_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_list_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_position_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_size_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_layout_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_state_t);
  ECS_COMPONENT_DEFINE(ecs_ctx, gui_event_handle_t);
  ECS_SYSTEM(ecs_ctx, gui_plugin_input_system, InkOnInput, plugin_t, GuiPlugin);
  //ECS_SYSTEM(ecs_ctx, gui_plugin_update_system, InkOnUpdate, GuiRoot);
  ecs_system(ecs_ctx, {
    .entity = ecs_entity(ecs_ctx, {
      .name = "GuiPluginUpdateSystem",
      .add = { ecs_dependson(InkOnUpdate) }
     }),
    .query.filter.terms = {
      { .id = GuiRoot },
    },
    .callback = gui_plugin_update_system,
    .no_readonly = true
  });
  ecs_entity_t plugin = ecs_new_id(ecs_ctx);
  gui_plugin_t *plug = malloc(sizeof(gui_plugin_t));
  ecs_add(ecs_ctx, plugin, GuiPlugin);
  ecs_set(ecs_ctx, plugin, plugin_t, {
    .eng = eng,
    .plugin = plug,
    .load = &gui_plugin_load,
    .unload = &gui_plugin_unload
  });

  const gui_nk_context_t *gui_nk_ctx = ecs_singleton_get(ecs_ctx, gui_nk_context_t);
  struct nk_context *nk_ctx = gui_nk_ctx->nk_ctx;
  Texture2D gui_texture = LoadTexture("engine/resources/textures/inkimera_gui.png");
  ecs_singleton_set(ecs_ctx, gui_texture_t, {
    .texture = gui_texture
  });
  struct nk_color table[NK_COLOR_COUNT];
  table[NK_COLOR_TEXT] = INK_WHITE;
  table[NK_COLOR_WINDOW] = INK_TRANSPARENT;
  table[NK_COLOR_HEADER] = INK_TRANSPARENT;
  table[NK_COLOR_BORDER] = INK_BLACK_INK;
  table[NK_COLOR_BUTTON] = INK_BLACK;
  table[NK_COLOR_BUTTON_HOVER] = INK_DARK_GREY;
  table[NK_COLOR_BUTTON_ACTIVE] = INK_DARK_GREY;
  table[NK_COLOR_TOGGLE] = INK_WHITE;
  table[NK_COLOR_TOGGLE_HOVER] = INK_WHITE;
  table[NK_COLOR_TOGGLE_CURSOR] = INK_WHITE;
  table[NK_COLOR_SELECT] = INK_TRANSPARENT;
  table[NK_COLOR_SELECT_ACTIVE] = INK_BLACK;
  table[NK_COLOR_SLIDER] = INK_WHITE;
  table[NK_COLOR_SLIDER_CURSOR] = INK_WHITE;
  table[NK_COLOR_SLIDER_CURSOR_HOVER] = INK_WHITE;
  table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = INK_WHITE;
  table[NK_COLOR_PROPERTY] = INK_WHITE;
  table[NK_COLOR_EDIT] = INK_BLACK;
  table[NK_COLOR_EDIT_CURSOR] = INK_WHITE;
  table[NK_COLOR_COMBO] = INK_BLACK_INK;
  table[NK_COLOR_CHART] = INK_WHITE;
  table[NK_COLOR_CHART_COLOR] = INK_WHITE;
  table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = INK_WHITE;
  table[NK_COLOR_SCROLLBAR] = INK_WHITE;
  table[NK_COLOR_SCROLLBAR_CURSOR] = INK_BLACK;
  table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = INK_GREY;
  table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = INK_GREY;
  table[NK_COLOR_TAB_HEADER] = INK_DARK_GREY;
  nk_style_from_table(nk_ctx, table);
  // WINDOW
  nk_ctx->style.window.spacing = nk_vec2(0, 0);
  nk_ctx->style.window.border = 0;
  nk_ctx->style.window.combo_border = 1;
  nk_ctx->style.window.contextual_border = 1;
  nk_ctx->style.window.menu_border = 1;
  nk_ctx->style.window.group_border = 0;
  nk_ctx->style.window.tooltip_border = 1;
  nk_ctx->style.window.popup_border = 1;
  nk_ctx->style.window.min_row_height_padding = 0;
  nk_ctx->style.window.padding = nk_vec2(0, 0);
  nk_ctx->style.window.group_padding = nk_vec2(0, 0);
  nk_ctx->style.window.popup_padding = nk_vec2(0, 0);
  nk_ctx->style.window.combo_padding = nk_vec2(0, 0);
  nk_ctx->style.window.contextual_padding = nk_vec2(0, 0);
  nk_ctx->style.window.menu_padding = nk_vec2(0, 0);
  nk_ctx->style.window.tooltip_padding = nk_vec2(0, 0);
  //nk_ctx->style.combo.button_padding = nk_vec2(0, 0);
  //nk_ctx->style.combo.button.padding = nk_vec2(0, 0);
  //nk_ctx->style.window.fixed_background = nk_style_item_image(nk_image_id(gui_texture.id));
  //nk_ctx->style.window.fixed_background = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 1024, 1024, 1024), 4, 2, 6, 8));
  //eng->nk_ctx->style.window.fixed_background = nk_style_item_image(nk_subimage_id(eng->gui.id, 512, 512, nk_rect(0, 0, 512, 512)));
  nk_ctx->style.tab.sym_minimize = NK_SYMBOL_NONE;
  nk_ctx->style.tab.sym_maximize = NK_SYMBOL_NONE;
  nk_ctx->style.tab.tab_minimize_button.normal = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.tab_minimize_button.hover = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.tab_minimize_button.active = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 512, 32, 32), 4, 2, 6, 8));

  nk_ctx->style.tab.tab_maximize_button.normal = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(32, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.tab_maximize_button.hover = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(32, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.tab_maximize_button.active = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(32, 512, 32, 32), 4, 2, 6, 8));

  nk_ctx->style.tab.node_minimize_button.normal = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.node_minimize_button.hover = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.node_minimize_button.active = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(0, 512, 32, 32), 4, 2, 6, 8));

  nk_ctx->style.tab.node_maximize_button.normal = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(32, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.node_maximize_button.hover = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(32, 512, 32, 32), 4, 2, 6, 8));
  nk_ctx->style.tab.node_maximize_button.active = nk_style_item_nine_slice(nk_sub9slice_id(gui_texture.id, 2048, 2048, nk_rect(32, 512, 32, 32), 4, 2, 6, 8));
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
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  node_id_t window = gui_new_window(eng, 0, 0, 1.0, 1.0, "Inkimera Editor");

  ecs_entity_t project = ecs_new_id(ecs_ctx);
  ecs_set(ecs_ctx, project, project_t, {
    .name = "untitled",
    .path = { 0 },
    .num_scenes = 1,
    .scenes = {
      (project_scene_t) { .name = "untitled" },
    }
  });
  ecs_add_pair(ecs_ctx, window, ProjectOf, project);
  ecs_add_pair(ecs_ctx, project, ProjectFor, window);

  ecs_entity_t scene = ecs_new_id(ecs_ctx);
  ecs_add(ecs_ctx, scene, ThreeDScene);
  ecs_set(ecs_ctx, scene, threed_scene_t, {
    .scene_id = gui_hash("untitled"),
    .name = "untitled",
  });
  ecs_add_pair(ecs_ctx, scene, EcsChildOf, project);

  node_id_t pane = gui_new_pane(eng, "", window);
  ecs_set(ecs_ctx, pane, gui_anchor_t, { GUI_ANCHOR_ROOT });
  ecs_add(ecs_ctx, pane, GuiFocus);

  gui_new_widget(ecs_ctx, pane, GUI_WIDGET_SCENE);

  gui_plug->window = window;
  gui_plug->focus = pane;
  gui_plug->timestamp = time(NULL);

  // Setup
  state_machine_state_t *idle_state = state_machine_add_state(&gui_plug->state, "idle");
  state_machine_state_t *leader_key_held_state = state_machine_add_state(&gui_plug->state, "leader_key_held");
  state_machine_state_t *leader_key_released_state = state_machine_add_state(&gui_plug->state, "leader_key_released");
  //state_machine_state_t *command_state = state_machine_add_state(&gui_plug->state, "command");

  state_machine_event_t *event_leader_held =
    state_machine_add_event(&gui_plug->state, (int[2]) { INK_KEY_LEFT_CONTROL, INK_KEY_A }, (int[2]) { INK_KEY_STATE_DOWN, INK_KEY_STATE_DOWN }, 2);
  state_machine_event_t *event_leader_released = state_machine_add_event(&gui_plug->state, (int[2]) { INK_KEY_LEFT_CONTROL, INK_KEY_A }, (int[2]) { INK_KEY_STATE_RELEASED | INK_KEY_STATE_UP,
                                                                                                                                                    INK_KEY_STATE_RELEASED | INK_KEY_STATE_UP }, 2);
  state_machine_event_t *event_esc = state_machine_add_event(&gui_plug->state, (int[1]) { INK_KEY_ESCAPE }, (int[1]) { INK_KEY_STATE_DOWN | INK_KEY_STATE_PRESSED }, 1);

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
