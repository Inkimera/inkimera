#ifndef THREED_COMPONENTS_H
#define THREED_COMPONENTS_H

#include "engine.h"

typedef struct {
  Vector3 position;
  Vector3 rotation;
  Vector3 scale;
} threed_transform_t;

/* threed_components_init */
int
threed_components_init(
  engine_t *eng
);

/* threed_node_make_scene */
int
threed_node_make_scene(
  engine_t *eng,
  node_id_t node
);
#endif
