#ifndef THREED_SYSTEMS_H
#define THREED_SYSTEMS_H

#include "flecs.h"

#include "engine.h"

/* threed_systems_init */
int
threed_systems_init(
  engine_t *eng
);

/* threed_node_system_pre */
void
threed_node_system_pre(
  ecs_iter_t *it
);

/* threed_node_system_post */
void
threed_node_system_post(
  ecs_iter_t *it
);

/* threed_node_system */
void
threed_node_system(
  ecs_iter_t *it
);

/* threed_render_node_system */
void
threed_render_node_system(
  ecs_iter_t *it
);
#endif
