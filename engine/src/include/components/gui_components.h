#ifndef GUI_COMPONENTS_H
#define GUI_COMPONENTS_H
#include "engine.h"

/* gui_components_init*/
int
gui_components_init(
  engine_t *eng
);

/* gui_node_clear */
int
gui_node_clear(
  engine_t *eng,
  node_id_t node
);

/* gui_node_get_type */
gui_node_type_t
gui_node_get_type(
  engine_t *eng,
  node_id_t node
);

/* gui_node_set_type */
int
gui_node_set_type(
  engine_t *eng,
  node_id_t node,
  gui_node_type_t type
);
#endif
