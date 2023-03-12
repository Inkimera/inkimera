#define RAYGUI_IMPLEMENTATION

#include <stdlib.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"
#include "raygui.h"

#include "hashmap.h"
#include "signal.h"

#include "engine.h"
#include "components/plugin_components.h"
#include "components/gui_components.h"
#include "components/3d_components.h"
#include "systems/gui_systems.h"
#include "systems/3d_systems.h"

#define int_t int
DECLARE_HASHMAP(int_t)
DEFINE_HASHMAP(int_t)

#define MAX_PLUGINS 100

/*
 * ENGINE CORE
 */
#define INKIMERA_STYLE_PROPS_COUNT  15

static const GuiStyleProp inkimera_style_props[INKIMERA_STYLE_PROPS_COUNT] = {
  { 0, 0, 0xab9bd3ff }, // DEFAULT_BORDER_COLOR_NORMAL
  { 0, 1, 0x3e4350ff }, // DEFAULT_BASE_COLOR_NORMAL
  { 0, 2, 0xdadaf4ff }, // DEFAULT_TEXT_COLOR_NORMAL
  { 0, 3, 0xee84a0ff }, // DEFAULT_BORDER_COLOR_FOCUSED
  { 0, 4, 0xf4b7c7ff }, // DEFAULT_BASE_COLOR_FOCUSED
  { 0, 5, 0xb7657bff }, // DEFAULT_TEXT_COLOR_FOCUSED
  { 0, 6, 0xd5c8dbff }, // DEFAULT_BORDER_COLOR_PRESSED
  { 0, 7, 0x966ec0ff }, // DEFAULT_BASE_COLOR_PRESSED
  { 0, 8, 0xd7ccf7ff }, // DEFAULT_TEXT_COLOR_PRESSED
  { 0, 9, 0x8fa2bdff }, // DEFAULT_BORDER_COLOR_DISABLED
  { 0, 10, 0x6b798dff }, // DEFAULT_BASE_COLOR_DISABLED
  { 0, 11, 0x8292a9ff }, // DEFAULT_TEXT_COLOR_DISABLED
  { 0, 16, 0x00000010 }, // DEFAULT_TEXT_SIZE
  { 0, 18, 0x84adb7ff }, // DEFAULT_LINE_COLOR
  { 0, 19, 0x5b5b81ff }, // DEFAULT_BACKGROUND_COLOR
};

/* engine_init */
engine_t*
engine_init(
  void
) {
  const int screenWidth = GetRenderHeight();
  const int screenHeight = GetRenderWidth();
  InitWindow(screenWidth, screenHeight, "Inkimera");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetExitKey(KEY_NULL);

  engine_t *eng = malloc(sizeof(engine_t));
  //eng->plugin_count = 0;
  //eng->plugins =
  //  malloc(MAX_PLUGINS * sizeof(plugin_handle_t*));
  eng->ecs_ctx = ecs_init();
  int fontSize = 14;
  eng->font = LoadFontEx("engine/resources/fonts/IBMPlexMono-Regular.ttf", fontSize, NULL, 0);
  for (int i = 0; i < INKIMERA_STYLE_PROPS_COUNT; i++) {
    GuiSetStyle(inkimera_style_props[i].controlId, inkimera_style_props[i].propertyId, inkimera_style_props[i].propertyValue);
  }
  GuiSetFont(eng->font);
  SetTextureFilter(eng->font.texture, TEXTURE_FILTER_POINT);
  SetTargetFPS(60);
  ecs_set_target_fps(eng->ecs_ctx, 60);

  // Init ecs systems
  plugin_components_init(eng);
  gui_components_init(eng);
  threed_components_init(eng);
  gui_systems_init(eng);
  threed_systems_init(eng);
  return eng;
}

/* engine_deinit */
int
engine_deinit(
  engine_t *eng
) {
  CloseWindow();
  /*
     while (eng->plugin_count-- > 0) {
     plugin_handle_t *handle =
      eng->plugins[eng->plugin_count];
     handle->unload(eng, handle->plugin);
     free(handle);
     eng->plugins[eng->plugin_count] = NULL;
     }
   */
  ecs_fini(eng->ecs_ctx);
  UnloadFont(eng->font);
  free(eng);
  return 0;
}

/*
 * INPUT
 */

/* engine_key_state_get */
engine_key_state_t
engine_key_state_get(
  engine_key_t key
) {
  int state = 0;
  if (IsKeyDown((int)key)) {
    state |= ENGINE_KEY_STATE_DOWN;
  }
  if (IsKeyPressed((int)key)) {
    state |= ENGINE_KEY_STATE_PRESSED;
  }
  if (IsKeyReleased((int)key)) {
    state |= ENGINE_KEY_STATE_RELEASED;
  }
  if (IsKeyUp((int)key)) {
    state |= ENGINE_KEY_STATE_UP;
  }
  return state;
}

/*
 * SIGNALS
 */

/*
 * ENGINE ECS
 */

/* engine_ecs_context */
ecs_world_t*
engine_ecs_context(
  engine_t *eng
) {
  return eng->ecs_ctx;
}

/* engine_ecs_get_children_of */
int
engine_ecs_get_children_of(
  engine_t *eng,
  node_id_t node,
  node_id_t *children,
  int max_children
) {
  ecs_world_t *ecs_ctx = eng->ecs_ctx;
  ecs_iter_t it = ecs_children(ecs_ctx, node);
  int num_children = 0;
  while (ecs_children_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      if (num_children >= max_children) {
        return -1;
      }
      ecs_entity_t child = it.entities[i];
      if (ecs_is_alive(ecs_ctx, child)) {
        children[num_children++] = child;
      }
    }
  }
  return num_children;
}

/*
 * ENGINE ECS (GUI)
 */

/* engine_gui_node_get_type */
gui_node_type_t
engine_gui_node_get_type(
  engine_t *eng,
  node_id_t node
) {
  return gui_node_get_type(eng, node);
}

/* engine_gui_node_set_type */
int
engine_gui_node_set_type(
  engine_t *eng,
  node_id_t node,
  gui_node_type_t type
) {
  return gui_node_set_type(eng, node, type);
}

/*
 * ENGINE ECS (3D)
 */

/* engine_threed_node_make_scene */
int
engine_threed_node_make_scene(
  engine_t *eng,
  node_id_t node
) {
  return threed_node_make_scene(eng, node);
}

/*
 * ENGINE RUNTIME
 */

/* engine_run */
void
engine_run(
  engine_t *eng
) {
  while (!WindowShouldClose()) {
    /*
       int i = 0;
       while (i < eng->plugin_count) {
       plugin_handle_t *handle = eng->plugins[i++];
       handle->update(eng, handle->plugin);
       }
     */

    // Render
    BeginDrawing();
    ClearBackground((Color) { 255, 0, 0, 0 });
    ecs_progress(eng->ecs_ctx, 0);
    EndDrawing();
  }
}
