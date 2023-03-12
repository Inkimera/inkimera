/* ThreeDNode */
extern ECS_TAG_DECLARE(ThreeDNodeActive);

/* threed_scene_t */
typedef struct {
  int scene_id;
} threed_scene_t;
extern ECS_COMPONENT_DECLARE(threed_scale_t);

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

/* threed_camera_t */
typedef struct {
  float fov;
} threed_camera_t;
extern ECS_COMPONENT_DECLARE(threed_camera_t);
