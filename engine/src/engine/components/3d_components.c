#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"

#include "engine.h"
#include "components/3d_components.h"

/* ThreeDNodeActive */
ECS_TAG_DECLARE(ThreeDNodeActive);

/* ThreeDNodeEditorOnly */
ECS_TAG_DECLARE(ThreeDNodeEditorOnly);

/* threed_node_type_t */
ECS_COMPONENT_DECLARE(threed_node_type_t);

/* threed_scene_t */
ECS_COMPONENT_DECLARE(threed_scene_t);

/* threed_render_texture_t */
ECS_COMPONENT_DECLARE(threed_render_texture_t);

/* threed_position_t */
ECS_COMPONENT_DECLARE(threed_position_t);

/* threed_rotation_t */
ECS_COMPONENT_DECLARE(threed_rotation_t);

/* threed_scale_t */
ECS_COMPONENT_DECLARE(threed_scale_t);

/* CameraOf */
ECS_ENTITY_DECLARE(CameraOf);

/* ActiveCameraFor */
ECS_ENTITY_DECLARE(ActiveCameraFor);

/* threed_camera_t */
ECS_COMPONENT_DECLARE(threed_camera_t);

/* threed_viewport_t */
ECS_COMPONENT_DECLARE(threed_viewport_t);

//ECS_CTOR(threed_render_texture_t, texture, {
//  printf("threed_render_texture_t(%p) load\n", (void*)texture);
//  texture->texture = (RenderTexture) { 0 };
//})
//
//ECS_DTOR(threed_render_texture_t, texture, {
//  printf("threed_render_texture_t(%p) unload\n", (void*)texture);
//  UnloadRenderTexture(texture->texture);
//})
//
//ECS_MOVE(threed_render_texture_t, dst, src, {
//  printf("threed_render_texture_t(%p) move\n", (void*)src);
//
//})
//
//ECS_COPY(threed_render_texture_t, dst, src, {
//  printf("threed_render_texture_t(%p) copy\n", (void*)src);
//})

void
threed_render_texture_on_remove(
  ecs_iter_t *it
) {
  ecs_world_t *ecs_ctx = it->world;
  ecs_entity_t event = it->event;
  for (int i = 0; i < it->count; i++) {
    ecs_entity_t e = it->entities[i];
    const threed_render_texture_t *texture = ecs_get(ecs_ctx, e, threed_render_texture_t);
    printf("threed_render_texture_on_remove(%p)\n", (void*)texture);
    UnloadRenderTexture(texture->texture);
  }
}

/* threed_components_init */
int
threed_components_init(
  engine_t *eng
) {
  ECS_TAG_DEFINE(eng->ecs_ctx, ThreeDNodeActive);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_scene_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_render_texture_t);
  ecs_set_hooks(eng->ecs_ctx, threed_render_texture_t, {
    //.ctor = ecs_ctor(threed_render_texture_t),
    //.dtor = ecs_dtor(threed_render_texture_t),
    //.move = ecs_move(threed_render_texture_t),
    //.copy = ecs_copy(threed_render_texture_t)
    .on_remove = threed_render_texture_on_remove
  });

  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_position_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_rotation_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_scale_t);
  ECS_ENTITY_DEFINE(eng->ecs_ctx, CameraOf);
  ECS_ENTITY_DEFINE(eng->ecs_ctx, ActiveCameraFor);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_camera_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_viewport_t);
  return 0;
}
