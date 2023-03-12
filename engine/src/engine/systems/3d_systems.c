#include <stdlib.h>
#include <stdio.h>

#include "flecs.h"

#include "engine.h"
#include "systems/3d_systems.h"
#include "components/3d_components.h"

/* threed_systems_init */
int
threed_systems_init(
  engine_t *eng
) {
  ECS_SYSTEM(eng->ecs_ctx, threed_node_system, EcsOnUpdate, ThreeDNodeActive);
  ECS_SYSTEM(eng->ecs_ctx, threed_render_node_system, EcsOnStore, threed_scene_t, threed_render_texture_t, ThreeDNodeActive);

  return 0;
}

// Scene drawing
void
DrawScene(
  void
) {
  int count = 5;
  float spacing = 4;

  // Grid of cube trees on a plane to make a "world"
  DrawPlane((Vector3) { 0, 0, 0 }, (Vector2) { 50, 50 }, BEIGE); // Simple world plane
  for (float x = -count * spacing; x <= count * spacing; x += spacing) {
    for (float z = -count * spacing; z <= count * spacing; z += spacing) {
      DrawCube((Vector3) { x, 1.5f, z }, 1, 1, 1, LIME);
      DrawCube((Vector3) { x, 0.5f, z }, 0.25f, 1, 0.25f, BROWN);
    }
  }
}

/* threed_update_graph */
static void
threed_update_graph(
  ecs_world_t *ecs_ctx,
  ecs_entity_t node
) {
  //printf("threed_update_graph\n");
}

/* threed_build_render_graph */
static void
threed_build_render_graph(
  ecs_world_t *ecs_ctx,
  const threed_scene_t *scene,
  ecs_entity_t node
) {
  //printf("threed_build_render_graph\n");
  DrawScene();
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

/* threed_render_node_system */
void
threed_render_node_system(
  ecs_iter_t *it
) {
  const threed_scene_t *scenes = ecs_field(it, threed_scene_t, 1);
  const threed_render_texture_t *render_textures = ecs_field(it, threed_render_texture_t, 2);

  Camera camera = { 0 };
  camera.fovy = 45.0f;
  camera.up.y = 1.0f;
  camera.target.y = 1.0f;
  camera.position.z = -3.0f;
  camera.position.y = 1.0f;

  Rectangle screen_rect = { .x = 0.0, .y = 0.0, .width = 1280.0, .height = -720.0 };
  for (int i = 0; i < it->count; i++) {
    BeginTextureMode(render_textures[i].texture);
    ClearBackground(SKYBLUE);
    BeginMode3D(camera);
    threed_build_render_graph(it->world, &scenes[i], it->entities[i]);
    EndMode3D();
    DrawText("PLAYER1 W/S to move", 10, 10, 20, RED);
    EndTextureMode();
  }
}
