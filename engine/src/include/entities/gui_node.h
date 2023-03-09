#include "flecs.h"
#include "engine.h"

/* gui_system_init */
int
gui_system_init(engine_t *eng);

/* gui_node_init */
node_id_t
gui_node_init(engine_t *eng);

/* gui_node_clear */
int
gui_node_clear(engine_t *eng, node_id_t node);

/* gui_node_get_type */
gui_node_type_t
gui_node_get_type(engine_t *eng, node_id_t node);

/* gui_node_set_type */
int
gui_node_set_type(engine_t *eng, node_id_t node, gui_node_type_t type);

/* gui_node_set_label */
int
gui_node_set_label(
  engine_t *eng,
  node_id_t node,
  gui_label_t label
);

/* gui_node_set_position */
int
gui_node_set_position(
  engine_t *eng,
  node_id_t node,
  gui_position_t position
);

/* gui_node_set_size */
int
gui_node_set_size(
  engine_t *eng,
  node_id_t node,
  gui_size_t size
);

/* gui_node_set_layout */
int
gui_node_set_layout(
  engine_t *eng,
  node_id_t node,
  gui_layout_t layout
);

/* gui_node_system */
void
gui_node_system(ecs_iter_t *it);
