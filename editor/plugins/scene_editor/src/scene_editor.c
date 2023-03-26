#include <stdlib.h>

#include "scene_editor.h"

#define PAN_SENSITIVITY 0.01
#define ROTATE_SENSITIVITY 0.001
#define ACCEL 0.4
#define DECEL 0.05

/* SceneEditorPlugin */
ECS_TAG_DECLARE(SceneEditorPlugin);

/* scene_editor_plugin_init */
node_id_t
scene_editor_plugin_init(
  engine_t *eng
) {
  printf("INIT scene_editor plugin\n");
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  ECS_TAG_DEFINE(ecs_ctx, SceneEditorPlugin);
  ECS_SYSTEM(ecs_ctx, scene_editor_scene_system, InkOnUpdate, threed_camera_t, threed_viewport_t, threed_position_t, threed_rotation_t, threed_velocity_t, threed_spin_t);

  ecs_entity_t plugin = ecs_new_id(ecs_ctx);
  scene_editor_plugin_t *plug = malloc(sizeof(scene_editor_plugin_t));
  ecs_add(ecs_ctx, plugin, SceneEditorPlugin);
  ecs_set(ecs_ctx, plugin, plugin_t, {
    .eng = eng,
    .plugin = plug,
    .load = &scene_editor_plugin_load,
    .unload = &scene_editor_plugin_unload
  });
  return plugin;
}

/* scene_editor_plugin_deinit */
int
scene_editor_plugin_deinit(
  scene_editor_plugin_t *plug
) {
  printf("DEINIT scene_editor plugin\n");
  free(plug);
  return 0;
}

/* scene_editor_plugin_load */
int
scene_editor_plugin_load(
  engine_t *eng,
  void *plug
) {
  printf("LOAD scene_editor plugin\n");
  ecs_world_t *ecs_ctx = ink_ecs_context(eng);
  // Scene
  //ecs_entity_t scene = ecs_new_id(ecs_ctx);
  //ecs_add(ecs_ctx, scene, ThreeDScene);
  //printf("Scene Node(%lld)\n", (long long)scene);
  return 0;
}

/* scene_editor_plugin_unload */
int
scene_editor_plugin_unload(
  engine_t *eng,
  void *plug
) {
  printf("UNLOAD scene_editor plugin\n");
  (void)eng;
  scene_editor_plugin_t *scene_editor_plug = (scene_editor_plugin_t*)plug;
  return scene_editor_plugin_deinit(scene_editor_plug);
}

/* scene_editor_contains_mouse */
static bool
scene_editor_contains_mouse(
  int mouse_x,
  int mouse_y,
  int viewport_width,
  int viewport_height
) {
  if (mouse_x >= 1 && mouse_x < viewport_width && mouse_y >= 0 && mouse_y < viewport_height) {
    return true;
  } else {
    return false;
  }
}

/* scene_editor_pan_camera */
static void
scene_editor_pan_camera(
  ecs_world_t *ecs_ctx,
  ecs_entity_t camera_node,
  threed_camera_t *camera,
  threed_position_t *position,
  threed_rotation_t *rotation,
  Vector2 mouse_delta
) {
  Quaternion rot = QuaternionFromEuler(rotation->x * DEG2RAD, rotation->y * DEG2RAD, 0.0);
  Vector3 forward = Vector3RotateByQuaternion(camera->forward, rot);
  Vector3 right = Vector3RotateByQuaternion(camera->right, rot);

  Vector3 up = Vector3CrossProduct(forward, right);
  Vector3 up_mov = Vector3Scale(up, mouse_delta.y * PAN_SENSITIVITY);
  Vector3 right_mov = Vector3Scale(right, mouse_delta.x * PAN_SENSITIVITY);

  Vector3 pos = Vector3Add(*position, right_mov);
  pos = Vector3Add(pos, up_mov);
  Vector3 target = Vector3Add(camera->target, right_mov);
  target = Vector3Add(target, up_mov);

  camera->target = target;
  *position = pos;
}

/* scene_editor_rotate_camera */
static void
scene_editor_rotate_camera(
  ecs_world_t *ecs_ctx,
  ecs_entity_t camera_node,
  threed_camera_t *camera,
  threed_position_t *position,
  threed_rotation_t *rotation,
  float pitch,
  float yaw
) {
  rotation->x = rotation->x + (pitch * RAD2DEG);
  if (rotation->x > 89.9) {
    rotation->x = 89.9;
  } else if (rotation->x < -89.9) {
    rotation->x = -89.9;
  }
  rotation->y = rotation->y + (yaw * RAD2DEG);
  if (rotation->y > 360.0) {
    rotation->y -= 360.0;
  } else if (rotation->y < -360.0) {
    rotation->x += 360.0;
  }
  Quaternion rot = QuaternionFromEuler(rotation->x * DEG2RAD, rotation->y * DEG2RAD, 0.0);
  Vector3 forward = Vector3RotateByQuaternion(camera->forward, rot);
  Vector3 pos = Vector3Subtract(camera->target, Vector3Scale(forward, camera->distance));
  *position = pos;
}

/* scene_editor_zoom_camera */
static void
scene_editor_zoom_camera(
  ecs_world_t *ecs_ctx,
  ecs_entity_t camera_node,
  threed_camera_t *camera,
  threed_position_t *position,
  threed_rotation_t *rotation,
  float delta
) {
  camera->distance += delta;
  if (camera->distance < 0.0) {
    camera->distance = 0.1;
  }
  Quaternion rot = QuaternionFromEuler(rotation->x * DEG2RAD, rotation->y * DEG2RAD, 0.0);
  Vector3 forward = Vector3RotateByQuaternion(camera->forward, rot);
  Vector3 pos = Vector3Subtract(camera->target, Vector3Scale(forward, camera->distance));
  *position = pos;
}

/* scene_editor_system */
void
scene_editor_system(
  ecs_iter_t *it
) {
  const plugin_t *plugs = ecs_field(it, plugin_t, 1);
  engine_t *eng = plugs[0].eng;
  scene_editor_plugin_t *scene_editor_plug = (scene_editor_plugin_t*)plugs[0].plugin;
}

/* scene_editor_scene_system */
void
scene_editor_scene_system(
  ecs_iter_t *it
) {
  ecs_world_t *ecs_ctx = it->world;
  const threed_camera_t *cameras = ecs_field(it, threed_camera_t, 1);
  const threed_viewport_t *viewports = ecs_field(it, threed_viewport_t, 2);
  const threed_position_t *positions = ecs_field(it, threed_position_t, 3);
  const threed_rotation_t *rotations = ecs_field(it, threed_rotation_t, 4);
  const threed_velocity_t *velocities = ecs_field(it, threed_velocity_t, 5);
  const threed_spin_t *spins = ecs_field(it, threed_spin_t, 6);

  ink_key_state_t shift = ink_key_state_get(INK_KEY_LEFT_SHIFT)
    | ink_key_state_get(INK_KEY_RIGHT_SHIFT);
  ink_key_state_t mouse_middle = ink_mouse_state_get(INK_MOUSE_BUTTON_MIDDLE);
  Vector2 mouse_delta = GetMouseDelta();
  float mouse_wheel_delta = -GetMouseWheelMove();
  for (int i = 0; i < it->count; i++) {
    ecs_entity_t camera_node = it->entities[i];
    const threed_viewport_t viewport = viewports[i];
    const threed_camera_t camera = cameras[i];
    const threed_position_t camera_pos = positions[i];
    const threed_rotation_t camera_rot = rotations[i];
    const threed_velocity_t camera_velocity = velocities[i];
    const threed_spin_t camera_spin = spins[i];
    const int mouse_x = GetMouseX() - viewport.xoff;
    const int mouse_y = GetMouseY() - viewport.yoff;
    if (scene_editor_contains_mouse(mouse_x, mouse_y, viewport.width, viewport.height)) {
      threed_camera_t c = camera;
      threed_position_t p = camera_pos;
      threed_rotation_t r = camera_rot;
      Vector3 velocity = Vector3Lerp(camera_velocity, (Vector3) { 0 }, DECEL);
      Vector3 spin = Vector3Lerp(camera_spin, (Vector3) { 0 }, DECEL);
      if ((shift & INK_KEY_STATE_DOWN) && (mouse_middle & INK_KEY_STATE_DOWN)) {
        velocity = Vector3Lerp(velocity, (Vector3) { .x = mouse_delta.x, .y = mouse_delta.y }, ACCEL);
      } else if ((mouse_middle & INK_KEY_STATE_DOWN)) {
        spin = Vector3Lerp(spin, (Vector3) { .x = mouse_delta.x, .y = mouse_delta.y }, ACCEL);
      } else if (mouse_wheel_delta != 0.0) {
        scene_editor_zoom_camera(ecs_ctx, camera_node, &c, &p, &r, mouse_wheel_delta);
      }
      if (Vector3LengthSqr(velocity) > 1.0) {
        scene_editor_pan_camera(ecs_ctx, camera_node, &c, &p, &r, (Vector2) { .x = velocity.x, .y = velocity.y });
      }
      if (Vector3LengthSqr(spin) > 1.0) {
        float yaw = -spin.x * ROTATE_SENSITIVITY;
        float pitch = spin.y * ROTATE_SENSITIVITY;
        scene_editor_rotate_camera(ecs_ctx, camera_node, &c, &p, &r, pitch, yaw);
      }
      ecs_set(ecs_ctx, camera_node, threed_camera_t, {
        .fov = c.fov,
        .distance = c.distance,
        .up = c.up,
        .right = c.right,
        .forward = c.forward,
        .target = c.target
      });
      ecs_set(ecs_ctx, camera_node, threed_position_t, {
        .x = p.x,
        .y = p.y,
        .z = p.z
      });
      ecs_set(ecs_ctx, camera_node, threed_rotation_t, {
        .x = r.x,
        .y = r.y,
        .z = r.z
      });
      ecs_set(ecs_ctx, camera_node, threed_velocity_t, {
        .x = velocity.x,
        .y = velocity.y,
        .z = velocity.z
      });
      ecs_set(ecs_ctx, camera_node, threed_spin_t, {
        .x = spin.x,
        .y = spin.y,
        .z = spin.z
      });
    }
  }
}
