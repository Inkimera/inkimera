#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
