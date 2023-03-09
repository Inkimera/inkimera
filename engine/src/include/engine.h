#ifndef ENGINE_H
#define ENGINE_H
#include "flecs.h"
#include "raylib.h"
#include "inkimera.h"
#include "signal.h"

/*
 * ENGINE
 */

/* engine_t */
struct engine {
  Font font;
  int plugin_count;
  plugin_handle_t **plugins;
  ecs_world_t *ecs_ctx;
};

/*
 * PLUGIN
 */

/* plugin_handle_t */
struct plugin_handle {
  void *plugin;
  load_handle_t load;
  unload_handle_t unload;
  update_handle_t update;
};
#endif
