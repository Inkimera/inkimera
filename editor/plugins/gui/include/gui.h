#ifndef GUI_H
#define GUI_H

#include <time.h>

#include "inkimera.h"
#include "keyboard_state_machine.h"

/* GuiPlugin */
extern ECS_TAG_DECLARE(GuiPlugin);

/* gui_plugin_t */
typedef struct {
  node_id_t window;
  node_id_t focus;
  state_machine_t state;
  time_t timestamp;
} gui_plugin_t;

/* gui_plugin_init */
node_id_t
gui_plugin_init(
  engine_t *eng
);

/* gui_plugin_deinit */
int
gui_plugin_deinit(
  gui_plugin_t *plug
);

/* gui_plugin_load */
int
gui_plugin_load(
  engine_t *eng,
  void *plug
);

/* gui_plugin_unload */
int
gui_plugin_unload(
  engine_t *eng,
  void *plug
);

/* gui_plugin_input_system */
void
gui_plugin_input_system(
  ecs_iter_t *it
);

/* gui_plugin_update_system */
void
gui_plugin_update_system(
  ecs_iter_t *it
);
#endif
