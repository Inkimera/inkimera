#include <stdlib.h>
#include <sys/stat.h>

#include "project.h"
#include "toml.h"

#define MAX_PROJECT_SIZE 4096

#define TOML_BEGIN(file, table_name) fprintf(file, "[%s]\n", table_name)
#define TOML_BOOL(file, name, value) fprintf(file, "%s = %s\n", #name, (value) ? "true" : "false")
#define TOML_STRING(file, name, value) fprintf(file, "%s = \"%s\"\n", #name, value)
#define TOML_INT(file, name, value) fprintf(file, "%s = %i\n", #name, value)
#define TOML_FLOAT(file, name, value) fprintf(file, "%s = %f\n", #name, value)
#define TOML_BEGIN_TABLE_ARRAY(file, name) fprintf(file, "[[%s]]\n", name)
#define TOML_END_TABLE_ARRAY(file)
#define TOML_END(file) fclose(file)

/* ProjectOf */
ECS_ENTITY_DECLARE(ProjectOf);

/* ProjectFor */
ECS_ENTITY_DECLARE(ProjectFor);

/* project_scene_t */
ECS_COMPONENT_DECLARE(project_scene_t);

/* project_t */
ECS_COMPONENT_DECLARE(project_t);

/*
   const char *project_tmpl = "\
   [inkimera]\n\
   name = \"%s\"\n\
   ";

   const char *project_scene_tmpl = "\
   [[scene]]\n\
   name = \"%s\"\n\
   ";
 */

/* project_save */
bool
project_save(
  const project_t *project
) {
  //char project_path[MAX_PATH_SIZE + MAX_NAME_SIZE];
  //snprintf(project_path, MAX_PATH_SIZE + MAX_NAME_SIZE, "%s/project.ink", project->path);
  //printf("Project Path: %s\n", project_path);
  const char *project_dir = GetPrevDirectoryPath(project->path);
  if (!DirectoryExists(project_dir)) {
    if (mkdir(project_dir, 0777) != 0) {
      printf("Failed to write %s\n", project_dir);
      return false;
    }
  }
  FILE *fp = fopen(project->path, "w");
  TOML_BEGIN(fp, "inkimera");
  TOML_STRING(fp, name, project->name);
  for (int i = 0; i < project->num_scenes; i++) {
    if (!strcmp(project->scenes[i].path, "")) {
      continue;
    }
    TOML_BEGIN_TABLE_ARRAY(fp, "scene");
    TOML_STRING(fp, name, project->scenes[i].name);
    TOML_STRING(fp, path, project->scenes[i].path);
    TOML_END_TABLE_ARRAY(fp);
  }
  TOML_END(fp);

  return true;
  //char buffer[MAX_PROJECT_SIZE];
  //snprintf(buffer, MAX_PROJECT_SIZE, project_tmpl, project->name);
  //return SaveFileText(project_path, buffer);
}

/* project_load */
project_t
project_load(
  const char *path
) {
  project_t p = { 0 };
  strcpy(p.path, path);
  char err_buffer[200];
  FILE *fp = fopen(path, "r");
  if (!fp) {
    printf("Failed to load %s\n", path);
    return p;
  }
  toml_table_t *project = toml_parse_file(fp, err_buffer, sizeof(err_buffer));
  fclose(fp);

  toml_table_t *inkimera = toml_table_in(project, "inkimera");
  if (!inkimera) {
    printf("Invalid ink.project - missing [inkimera]\n");
  }
  toml_datum_t name = toml_string_in(inkimera, "name");
  if (!name.ok) {
    printf("Invalid ink.project - inkimera.name missing\n");
  } else {
    strcpy(p.name, name.u.s);
  }
  free(name.u.s);
  toml_array_t *scenes = toml_array_in(project, "scene");
  for (int i = 0; i < toml_array_nelem(scenes); i++) {
    toml_table_t *scene = toml_table_at(scenes, i);
    toml_datum_t scene_name = toml_string_in(scene, "name");
    toml_datum_t scene_path = toml_string_in(scene, "path");
    project_scene_t p_scene = { 0 };
    strcpy(p_scene.name, scene_name.u.s);
    strcpy(p_scene.path, scene_path.u.s);
    p.scenes[p.num_scenes] = p_scene;
    p.num_scenes++;
    free(scene_name.u.s);
    free(scene_path.u.s);
  }
  toml_free(project);
  printf("Loaded\n");
  return p;
}

/* project_save */
bool
project_scene_save(
  const char *dir,
  const project_scene_t *scene
) {
  //const char *project_dir = GetPrevDirectoryPath(dir);
  char scene_path[MAX_PATH_SIZE + MAX_NAME_SIZE];
  snprintf(scene_path, MAX_PATH_SIZE + MAX_NAME_SIZE, "%s/%s.ink", scene->path, scene->name);
  printf("Scene Path: %s\n", scene_path);
  const char *scene_dir = GetPrevDirectoryPath(scene_path);
  if (!DirectoryExists(scene_dir)) {
    if (mkdir(scene_dir, 0777) != 0) {
      printf("Failed to create_dir %s\n", scene_dir);
      return false;
    }
  }
  FILE *fp = fopen(scene_path, "w");
  if (!fp) {
    return false;
  }
  TOML_BEGIN(fp, "inkimera_scene");
  TOML_STRING(fp, name, scene->name);
  TOML_END(fp);
  return true;
}
