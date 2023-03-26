#ifndef INKIMERA_EDITOR_PROJECT_H
#define INKIMERA_EDITOR_PROJECT_H

#include "inkimera.h"

#define MAX_NAME_SIZE 128
#define MAX_PATH_SIZE 256
#define MAX_SCENES 16

/* ProjectOf */
extern ECS_ENTITY_DECLARE(ProjectOf);

/* ProjectFor */
extern ECS_ENTITY_DECLARE(ProjectFor);

/* project_scene_t */
typedef struct {
  char name[MAX_NAME_SIZE];
  char path[MAX_PATH_SIZE];
  bool saved;
} project_scene_t;
extern ECS_COMPONENT_DECLARE(project_scene_t);

/* project_t */
typedef struct {
  char name[MAX_NAME_SIZE];
  char path[MAX_PATH_SIZE];
  bool empty;
  bool saved;
  int num_scenes;
  project_scene_t scenes[MAX_SCENES];
} project_t;
extern ECS_COMPONENT_DECLARE(project_t);

/* project_save */
bool
project_save(
  const project_t *project
);

/* project_load */
project_t
project_load(
  const char *path
);

/* project_save */
bool
project_scene_save(
  const char *dir,
  const project_scene_t *scene
);
#endif
