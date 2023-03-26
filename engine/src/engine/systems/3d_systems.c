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
  threed_camera_t *camera,
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
      .position = *pos,
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
  Camera c = {
    .fovy = camera->fov,
    .up = Vector3Normalize(camera->up),
    .position = Vector3Transform(*camera_pos, camera_transform),
    .target = Vector3Transform(camera->target, camera_transform)
  };
  return c;
}

/* threed_node_system_pre */
void
threed_node_system_pre(
  ecs_iter_t *it
) {
  BeginDrawing();
  ClearBackground(PINK);
}

/* threed_node_system_post */
void
threed_node_system_post(
  ecs_iter_t *it
) {
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
  DrawCube((Vector3) { -1.0, 0.5, 0 }, 1, 1, 1, RED);
  DrawCube((Vector3) { 1.0, 0.5, 0 }, 1, 1, 1, GREEN);
  DrawCube((Vector3) { 0.0, 0.5, 1 }, 1, 1, 1, PINK);
}

/* threed_build_render_graph */
static void
threed_build_render_graph(
  ecs_world_t *ecs_ctx,
  const threed_scene_t *scene,
  ecs_entity_t node
) {
  //printf("threed_build_render_graph\n");
  //threed_draw_scene();
  const threed_position_t *position = ecs_get(ecs_ctx, node, threed_position_t);
  const threed_rotation_t *rotation = ecs_get(ecs_ctx, node, threed_rotation_t);
  rlPushMatrix();
  {
    rlScalef(1.0, 1.0, 1.0);
    rlTranslatef(position->x, position->y, position->z);
    rlRotatef(rotation->z, 0.0, 0.0, 1.0);
    rlRotatef(rotation->y, 0.0, 1.0, 0.0);
    rlRotatef(rotation->x, 1.0, 0.0, 0.0);
    DrawCube((Vector3) { 0.0, 0.0, 0.0 }, 1, 1, 1, (Color) { 217, 143, 143, 255 });
    ecs_iter_t it = ecs_children(ecs_ctx, node);
    while (ecs_children_next(&it)) {
      for (int i = 0; i < it.count; i++) {
        ecs_entity_t child = it.entities[i];
        threed_build_render_graph(ecs_ctx, scene, child);
      }
    }
  }
  rlPopMatrix();
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

    // Scene
    BeginTextureMode(render_texture.fbo);
    {
      ClearBackground((Color) { 217, 217, 217, 255 });
      BeginMode3D(camera);
      {
        DrawGrid(100.0, 1.0);
        ecs_iter_t it = ecs_children(ecs_ctx, scene_node);
        while (ecs_children_next(&it)) {
          for (int i = 0; i < it.count; i++) {
            ecs_entity_t child = it.entities[i];
            threed_build_render_graph(ecs_ctx, scene, child);
          }
        }
      }
      EndMode3D();
      //DrawFPS(2, 2);
      int fps = GetFPS();
      char scene_text[64];
      snprintf(scene_text, 64, "FPS: %i", fps);
      DrawTextEx(GetFontDefault(), scene_text, (Vector2) { 2, 2 }, 14, 4, (Color) { 217, 143, 143, 255 });
    }
    EndTextureMode();
    // Final image
    BeginTextureMode(render_texture.texture);
    {
      // Flip y-axis of fbo texture
      Rectangle source = { 0, 0, render_texture.texture.texture.width, render_texture.texture.texture.height };
      DrawTexturePro(render_texture.fbo.texture, source, source, (Vector2) { .x = 0.0, .y = 0.0 }, 0.0, WHITE);
    }
    EndTextureMode();
  }
}
