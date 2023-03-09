#ifndef GUI_H
#define GUI_H

#include <time.h>

#include "inkimera.h"
#include "keyboard_state_machine.h"

typedef struct {
  void *user_events_signal;
  node_id_t window;
  node_id_t focus;
  state_machine_t state;
  time_t timestamp;
} gui_plugin_t;

int
gui_plugin_load(
  engine_t *eng,
  void *plug
);
int
gui_plugin_unload(
  engine_t *eng,
  void *plug
);
void
gui_plugin_events(
  engine_t *eng,
  void *plug
);

gui_plugin_t*
gui_plugin_init();
int
gui_plugin_deinit(
  gui_plugin_t *plug
);
#endif
