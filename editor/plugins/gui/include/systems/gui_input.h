#ifndef GUI_INPUT_H
#define GUI_INPUT_H

#include "inkimera.h"

static char *PANE_ITEMS[] = { "SCENE", "TERM", "PROPS", "NODES" };
static int NUM_PANE_ITEMS = 4;

/* gui_search_direction_t */
typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT
} gui_search_direction_t;

/* gui_plugin_input_system */
void
gui_plugin_input_system(
  ecs_iter_t *it
);

/* gui_plugin_input_system */
void
gui_plugin_input_system(
  ecs_iter_t *it
);
#endif
