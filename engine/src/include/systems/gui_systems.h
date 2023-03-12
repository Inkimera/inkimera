#include "flecs.h"
#include "engine.h"

/* gui_system_init */
int
gui_systems_init(
  engine_t *eng
);

/* gui_node_input_system */
void
gui_node_input_system(
  ecs_iter_t *it
);

/* gui_node_update_system */
void
gui_node_update_system(
  ecs_iter_t *it
);

/* gui_node_render_system */
void
gui_node_render_system(
  ecs_iter_t *it
);
