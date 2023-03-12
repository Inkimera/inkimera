#ifndef INKIMERA_THREED_H
#define INKIMERA_THREED_H

/* ThreeDNodeActive */
extern ECS_TAG_DECLARE(ThreeDNodeActive);

/* ThreeDNodeEditorOnly */
extern ECS_TAG_DECLARE(ThreeDNodeEditorOnly);

/* threed_render_texture_t */
typedef struct {
  RenderTexture texture;
} threed_render_texture_t;
extern ECS_COMPONENT_DECLARE(threed_render_texture_t);

/* gui_node_type_t */
typedef enum {
  THREED_NODE,
  THREED_NODE_CAMERA
} threed_node_type_t;
extern ECS_COMPONENT_DECLARE(threed_node_type_t);

/* threed_scene_t */
typedef struct {
  int scene_id;
} threed_scene_t;
extern ECS_COMPONENT_DECLARE(threed_scene_t);

/* threed_position_t */
typedef struct {
  float x;
  float y;
  float z;
} threed_position_t;
extern ECS_COMPONENT_DECLARE(threed_position_t);

/* threed_rotation_t */
typedef struct {
  float x;
  float y;
  float z;
} threed_rotation_t;
extern ECS_COMPONENT_DECLARE(threed_rotation_t);

/* threed_scale_t */
typedef struct {
  float x;
  float y;
  float z;
} threed_scale_t;
extern ECS_COMPONENT_DECLARE(threed_scale_t);

/* CameraOf */
extern ECS_ENTITY_DECLARE(CameraOf);

/* ActiveCameraFor */
extern ECS_ENTITY_DECLARE(ActiveCameraFor);

/* threed_camera_t */
typedef struct {
  float fov;
  struct {
    float x;
    float y;
    float z;
  } up;
  struct {
    float x;
    float y;
    float z;
  } target;
} threed_camera_t;
extern ECS_COMPONENT_DECLARE(threed_camera_t);

/* threed_viewport_t */
typedef struct {
  int offx;
  int offy;
  int width;
  int height;
} threed_viewport_t;
extern ECS_COMPONENT_DECLARE(threed_viewport_t);
#endif
