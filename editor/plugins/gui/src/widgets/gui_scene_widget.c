#include "widgets/gui_scene_widget.h"

/* gui_scene_widget_add_camera */
static void
gui_scene_widget_add_camera(
  ecs_world_t *ecs_ctx,
  ecs_entity_t widget,
  ecs_entity_t scene,
  Rectangle container
) {
  // Camera
  ecs_entity_t camera = ecs_new_id(ecs_ctx);
  ecs_add(ecs_ctx, camera, ThreeDNodeActive);
  ecs_set(ecs_ctx, camera, threed_viewport_t, {
    .xoff = container.x,
    .yoff = container.y,
    .width = container.width,
    .height = container.height
  });
  ecs_set(ecs_ctx, camera, threed_camera_t, {
    .fov = 90.0,
    .distance = 1.0,
    .up = { .x = 0.0, .y = 1.0, .z = 0.0 },
    .right = { .x = 1.0, .y = 0.0, .z = 0.0 },
    .forward = { .x = 0.0, .y = 0.0, .z = 1.0 },
    .target = { .x = 0.0, .y = 0.0, .z = 0.0 }
  });
  ecs_set(ecs_ctx, camera, threed_render_texture_t, {
    .fbo = LoadRenderTexture(container.width, container.height),
    .texture = LoadRenderTexture(container.width, container.height)
  });
  ecs_set(ecs_ctx, camera, threed_position_t, { .x = 0.0, .y = 0.0, .z = -1.0 });
  ecs_set(ecs_ctx, camera, threed_rotation_t, { .x = 0.0, .y = 0.0, .z = 0.0 });
  ecs_set(ecs_ctx, camera, threed_velocity_t, { .x = 0.0, .y = 0.0, .z = 0.0 });
  ecs_set(ecs_ctx, camera, threed_spin_t, { .x = 0.0, .y = 0.0, .z = 0.0 });
  ecs_add_pair(ecs_ctx, camera, EcsChildOf, scene);
  ecs_add_pair(ecs_ctx, camera, CameraOf, scene);

  ecs_add_pair(ecs_ctx, widget, ActiveCameraFor, camera);
}

/* gui_scene_widget */
void
gui_scene_widget(
  ecs_world_t *ecs_ctx,
  struct nk_context *nk_ctx,
  Rectangle container,
  ecs_entity_t widget,
  ecs_entity_t scene
) {
  node_id_t camera = ecs_get_target(ecs_ctx, widget, ActiveCameraFor, 0);
  if (!camera ) {
    printf("Scene Node(%lld) has no camera\n", (long long)scene);
    return gui_scene_widget_add_camera(ecs_ctx, widget, scene, container);
  }
  const threed_render_texture_t *render_texture = ecs_get(ecs_ctx, camera, threed_render_texture_t);
  if (!render_texture) {
    printf("Invalid camera\n");
    return;
  }
  Rectangle rect = container;
  if (rect.width != render_texture->texture.texture.width || rect.height != render_texture->texture.texture.height) {
    printf("Resize viewport\n");
    //ecs_remove(ecs_ctx, camera, threed_render_texture_t);
    UnloadRenderTexture(render_texture->fbo);
    UnloadRenderTexture(render_texture->texture);

    ecs_set(ecs_ctx, camera, threed_viewport_t, {
      .xoff = container.x,
      .yoff = container.y,
      .width = container.width,
      .height = container.height
    });
    ecs_set(ecs_ctx, camera, threed_render_texture_t, {
      .fbo = LoadRenderTexture(container.width, container.height),
      .texture = LoadRenderTexture(container.width, container.height)
    });
  }
  char window_name[128] = { 0 };
  sprintf(window_name, "GUI_WIDGET_SCENE(%lld)", (long long)widget);
  if (nk_begin(nk_ctx, window_name, nk_rect(rect.x, rect.y, rect.width, rect.height), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND | NK_WINDOW_NO_INPUT)) {
    nk_layout_row_static(nk_ctx, container.height, container.width, 1);
    struct nk_image img = nk_image_id(render_texture->texture.texture.id);
    nk_image(nk_ctx, img);
  }
  nk_end(nk_ctx);
}
