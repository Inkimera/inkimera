#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"

#include "engine.h"
#include "components/3d_components.h"
#include "rlgl.h"

/* ThreeDScene */
ECS_TAG_DECLARE(ThreeDScene);

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

/* threed_velocity_t */
ECS_COMPONENT_DECLARE(threed_velocity_t);

/* threed_spin_t */
ECS_COMPONENT_DECLARE(threed_spin_t);

/* CameraOf */
ECS_ENTITY_DECLARE(CameraOf);

/* ActiveCameraFor */
ECS_ENTITY_DECLARE(ActiveCameraFor);

/* threed_camera_t */
ECS_COMPONENT_DECLARE(threed_camera_t);

/* ThreeDViewportDirty */
ECS_TAG_DECLARE(ThreeDViewportDirty);

/* threed_viewport_t */
ECS_COMPONENT_DECLARE(threed_viewport_t);

threed_mrt_buffer_t
threed_load_mrt_buffer(
  int width,
  int height
) {
  threed_mrt_buffer_t target = { 0 };
  target.id = rlLoadFramebuffer(width, height);
  if (target.id > 0) {
    rlEnableFramebuffer(target.id);

    target.tex_color.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    target.tex_color.width = width;
    target.tex_color.height = height;
    target.tex_color.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.tex_color.mipmaps = 1;

    target.tex_normal.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    target.tex_normal.width = width;
    target.tex_normal.height = height;
    target.tex_normal.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.tex_normal.mipmaps = 1;

    target.tex_edge.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    target.tex_edge.width = width;
    target.tex_edge.height = height;
    target.tex_edge.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.tex_edge.mipmaps = 1;

    target.tex_depth.id = rlLoadTextureDepth(width, height, false);
    target.tex_depth.width = width;
    target.tex_depth.height = height;
    target.tex_depth.format = 19; // DEPTH_COMPONENT_24BIT?
    target.tex_depth.mipmaps = 1;

    // Attach color textures and depth textures to FBO
    rlFramebufferAttach(target.id, target.tex_color.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(target.id, target.tex_normal.id, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(target.id, target.tex_edge.id, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);
    rlFramebufferAttach(target.id, target.tex_depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

    // Activate required color draw buffers
    rlActiveDrawBuffers(3);
    // Check if fbo is complete with attachments (valid)
    if (rlFramebufferComplete(target.id)) {
      TRACELOG(LOG_INFO, "FBO: [ID %i] MultiRenderTexture loaded successfully", target.id);
    }
    rlDisableFramebuffer();
  } else {
    TRACELOG(LOG_WARNING, "FBO: MultiRenderTexture can not be created");
  }
  return target;
}

// Unload multi render texture from GPU memory (VRAM)
void
threed_unload_mtr_buffer(
  threed_mrt_buffer_t target
) {
  if (target.id > 0) {
    rlUnloadTexture(target.tex_color.id);
    rlUnloadTexture(target.tex_normal.id);
    rlUnloadTexture(target.tex_edge.id);

    // NOTE: Depth texture is automatically queried
    // and deleted before deleting framebuffer
    rlUnloadFramebuffer(target.id);
  }
}

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
    UnloadRenderTexture(texture->fbo);
    UnloadRenderTexture(texture->texture);
  }
}

/* threed_components_init */
int
threed_components_init(
  engine_t *eng
) {
  ECS_TAG_DEFINE(eng->ecs_ctx, ThreeDScene);
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
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_velocity_t);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_spin_t);
  ECS_ENTITY_DEFINE(eng->ecs_ctx, CameraOf);
  ECS_ENTITY_DEFINE(eng->ecs_ctx, ActiveCameraFor);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_camera_t);
  ECS_TAG_DEFINE(eng->ecs_ctx, ThreeDViewportDirty);
  ECS_COMPONENT_DEFINE(eng->ecs_ctx, threed_viewport_t);
  return 0;
}
