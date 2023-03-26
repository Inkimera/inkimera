#ifndef INKIMERA_THREED_H
#define INKIMERA_THREED_H

#define INK_SCENE_NAME_MAX_SIZE 128

/* ThreeDScene */
extern ECS_TAG_DECLARE(ThreeDScene);

/* ThreeDNodeActive */
extern ECS_TAG_DECLARE(ThreeDNodeActive);

/* ThreeDNodeEditorOnly */
extern ECS_TAG_DECLARE(ThreeDNodeEditorOnly);

/* threed_render_texture_t */
typedef struct {
  RenderTexture fbo;
  RenderTexture texture;
} threed_render_texture_t;
extern ECS_COMPONENT_DECLARE(threed_render_texture_t);

/* threed_mrt_buffer_t */
typedef struct {
  unsigned int id;
  int width;
  int height;
  Texture tex_color;
  Texture tex_normal;
  Texture tex_edge;
  Texture tex_depth;
} threed_mrt_buffer_t;

/* gui_node_type_t */
typedef enum {
  THREED_NODE,
  THREED_NODE_CAMERA
} threed_node_type_t;
extern ECS_COMPONENT_DECLARE(threed_node_type_t);

/* threed_scene_t */
typedef struct {
  unsigned long scene_id;
  char name[INK_SCENE_NAME_MAX_SIZE];
} threed_scene_t;
extern ECS_COMPONENT_DECLARE(threed_scene_t);

/* threed_position_t */
typedef struct Vector3 threed_position_t;
extern ECS_COMPONENT_DECLARE(threed_position_t);

/* threed_rotation_t */
typedef Vector3 threed_rotation_t;
extern ECS_COMPONENT_DECLARE(threed_rotation_t);

/* threed_scale_t */
typedef Vector3 threed_scale_t;
extern ECS_COMPONENT_DECLARE(threed_scale_t);

/* threed_velocity_t */
typedef Vector3 threed_velocity_t;
extern ECS_COMPONENT_DECLARE(threed_velocity_t);

/* threed_spin_t */
typedef Vector3 threed_spin_t;
extern ECS_COMPONENT_DECLARE(threed_spin_t);

/* CameraOf */
extern ECS_ENTITY_DECLARE(CameraOf);

/* ActiveCameraFor */
extern ECS_ENTITY_DECLARE(ActiveCameraFor);

/* threed_camera_t */
typedef struct {
  float fov;
  float distance;
  Vector3 up;
  Vector3 forward;
  Vector3 right;
  Vector3 target;
} threed_camera_t;
extern ECS_COMPONENT_DECLARE(threed_camera_t);

/* ThreeDViewportDirty */
extern ECS_TAG_DECLARE(ThreeDViewportDirty);

/* threed_viewport_t */
typedef struct {
  int xoff;
  int yoff;
  int width;
  int height;
} threed_viewport_t;
extern ECS_COMPONENT_DECLARE(threed_viewport_t);

threed_mrt_buffer_t
threed_load_mrt_buffer(
  int width,
  int height
);

void
threed_unload_mtr_buffer(
  threed_mrt_buffer_t target
);
#endif
