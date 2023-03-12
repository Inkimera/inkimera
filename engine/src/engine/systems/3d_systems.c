#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"

#include "engine.h"
#include "pipeline.h"
#include "systems/3d_systems.h"
#include "components/3d_components.h"
#include "rlgl.h"

/* threed_systems_init */
int
threed_systems_init(
  engine_t *eng
) {
  ECS_SYSTEM(eng->ecs_ctx, threed_node_system_pre, InkOnPreRender);
  ECS_SYSTEM(eng->ecs_ctx, threed_node_system_post, InkOnPostRender);
  ECS_SYSTEM(eng->ecs_ctx, threed_node_system, InkOnUpdate, ThreeDNodeActive);
  ECS_SYSTEM(eng->ecs_ctx, threed_render_node_system, InkOnRender, threed_camera_t, threed_position_t, threed_rotation_t, threed_render_texture_t, ThreeDNodeActive);

  return 0;
}

Camera
threed_get_scene_camera(
  ecs_world_t *ecs_ctx,
  node_id_t scene_node,
  node_id_t camera_node,
  threed_camera_t *camera_prop,
  threed_position_t *camera_pos,
  threed_rotation_t *camera_rot
) {
  Matrix camera_transform = MatrixIdentity();
  threed_transform_t camera_transforms[10] = { 0 };
  int j = 0;
  node_id_t parent = ecs_get_target(ecs_ctx, camera_node, EcsChildOf, 0);
  while (parent != 0 && parent != scene_node) {
    const threed_position_t *pos = ecs_get(ecs_ctx, parent, threed_position_t);
    const threed_rotation_t *rot = ecs_get(ecs_ctx, parent, threed_rotation_t);
    camera_transforms[j++] = (threed_transform_t) {
      .position = { .x = pos->x, .y = pos->y, .z = pos->z },
      .rotation = { .x = -rot->x * DEG2RAD, .y = -rot->y * DEG2RAD, .z = rot->z * DEG2RAD }
    };
    parent = ecs_get_target(ecs_ctx, parent, EcsChildOf, 0);
  }
  if (j > 0) {
    j--;
    do {
      threed_transform_t t = camera_transforms[j--];
      Matrix rot = MatrixRotateZYX(t.rotation);
      camera_transform = MatrixMultiply(rot, camera_transform);
      Matrix pos = MatrixTranslate(t.position.x, t.position.y, t.position.z);
      camera_transform = MatrixMultiply(pos, camera_transform);
    } while (j > 0);
  }
  Camera camera = { 0 };
  camera.fovy = camera_prop->fov;
  Vector3 up = { .x = camera_prop->up.x, .y = camera_prop->up.y, .z = camera_prop->up.z };
  camera.up = Vector3Normalize(up);
  Vector3 target = { .x = camera_prop->target.x, .y = camera_prop->target.y, .z = camera_prop->target.z };
  camera.target = target;
  Vector3 pos = { .x = camera_pos->x, .y = camera_pos->y, .z = camera_pos->z };
  camera.position = Vector3Transform(pos, camera_transform);
  camera.projection = CAMERA_PERSPECTIVE;

  Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
  Vector3 right = Vector3CrossProduct(up, forward);

  // Pitch
  Vector3 target_position = Vector3Subtract(camera.target, camera.position);
  target_position = Vector3RotateByAxisAngle(target_position, right, -camera_rot->x * DEG2RAD);
  camera.target = Vector3Add(camera.position, target_position);
  //// Yaw
  target_position = Vector3Subtract(camera.target, camera.position);
  target_position = Vector3RotateByAxisAngle(target_position, camera.up, -camera_rot->y * DEG2RAD);
  camera.target = Vector3Add(camera.position, target_position);
  // Roll
  // TODO
  return camera;
}


/* threed_node_system_pre */
void
threed_node_system_pre(
  ecs_iter_t *it
) {
  BeginDrawing();
  ClearBackground(BLACK);
}

/* threed_node_system_post */
void
threed_node_system_post(
  ecs_iter_t *it
) {
  DrawFPS(2, 2);
  EndDrawing();
}

/* threed_update_graph */
static void
threed_update_graph(
  ecs_world_t *ecs_ctx,
  ecs_entity_t node
) {
  //printf("threed_update_graph\n");
}

/* threed_node_system */
void
threed_node_system(
  ecs_iter_t *it
) {
  for (int i = 0; i < it->count; i++) {
    threed_update_graph(it->world, it->entities[i]);
  }
}

// Scene drawing
void
threed_draw_scene(
  void
) {
  //DrawPlane((Vector3) { 0, 0, 0 }, (Vector2) { 100, 100 }, BEIGE);
  DrawGrid(10.0, 1.0);
  DrawCube((Vector3) { -1, 0.5, 0 }, 1, 1, 1, RED);
  DrawCube((Vector3) { 1, 0.5, 0 }, 1, 1, 1, GREEN);
  DrawCube((Vector3) { 0, 0.5, 1 }, 1, 1, 1, PINK);
}

/* threed_build_render_graph */
static void
threed_build_render_graph(
  ecs_world_t *ecs_ctx,
  const threed_scene_t *scene,
  ecs_entity_t node
) {
  //printf("threed_build_render_graph\n");
  threed_draw_scene();
}

/* threed_render_node_system */
void
threed_render_node_system(
  ecs_iter_t *it
) {
  ecs_world_t *ecs_ctx = it->world;
  const threed_camera_t *camera_props = ecs_field(it, threed_camera_t, 1);
  const threed_position_t *positions = ecs_field(it, threed_position_t, 2);
  const threed_rotation_t *rotations = ecs_field(it, threed_rotation_t, 3);
  const threed_render_texture_t *render_textures = ecs_field(it, threed_render_texture_t, 4);
  for (int i = 0; i < it->count; i++) {
    node_id_t camera_node = it->entities[i];
    threed_camera_t camera_prop = camera_props[i];
    threed_position_t camera_pos = positions[i];
    threed_rotation_t camera_rot = rotations[i];
    threed_render_texture_t render_texture = render_textures[i];

    node_id_t scene_node = ecs_get_target(ecs_ctx, camera_node, CameraOf, 0);
    const threed_scene_t *scene = ecs_get(ecs_ctx, scene_node, threed_scene_t);

    Camera camera = threed_get_scene_camera(ecs_ctx, scene_node, camera_node, &camera_prop, &camera_pos, &camera_rot);

    BeginTextureMode(render_texture.texture);
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);
    threed_build_render_graph(ecs_ctx, scene, scene_node);
    EndMode3D();
    DrawTextEx(GetFontDefault(), "SCENE", (Vector2) { 10, 10 }, 20, 2, RED);
    //Rectangle source = { 0, 0, 1280, 720 };
    //Rectangle dest = { 0, 0, 1280, 720 };
    //DrawTexturePro(render_texture.texture.texture, source, dest, (Vector2) { .x = 0.0, .y = 0.0 }, 0.0, WHITE);
    EndTextureMode();

    // sub optimal way to flip y
    //Image img = LoadImageFromTexture(render_texture.texture.texture);
    //ImageFlipVertical(&img);
    //UpdateTexture(render_texture.texture.texture, img.data);
  }
}
