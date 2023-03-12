#ifndef THREED_COMPONENTS_H
#define THREED_COMPONENTS_H

#include "flecs.h"
#include "raylib.h"

#include "engine.h"

/* threed_render_texture_t */
typedef struct {
  RenderTexture texture;
} threed_render_texture_t;
extern ECS_COMPONENT_DECLARE(threed_render_texture_t);

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
