#include <stdlib.h>

#include "flecs.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "raylib.h"

#include "hashmap.h"
#include "signal.h"
#include "raylib_nuklear.h"

#include "engine.h"
#include "pipeline.h"
#include "components/plugin_components.h"
#include "components/gui_components.h"
#include "components/3d_components.h"
#include "systems/gui_systems.h"
#include "systems/3d_systems.h"

/*
 * ENGINE CORE
 */
/* engine_init */
engine_t*
engine_init(
  void
) {
  const int screenWidth = GetRenderHeight();
  const int screenHeight = GetRenderWidth();
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Inkimera");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetExitKey(KEY_NULL);

  engine_t *eng = malloc(sizeof(engine_t));
  // Init GUI
  int fontSize = 20;
  eng->font = LoadFontEx("engine/resources/fonts/IBMPlexMono-Regular.ttf", fontSize, NULL, 0);
  SetTextureFilter(eng->font.texture, TEXTURE_FILTER_POINT);
  eng->nk_ctx = nk_raylib_init(eng->font, (float)fontSize);
  eng->nk_ctx->style.window.spacing = nk_vec2(0, 0);
  eng->nk_ctx->style.window.combo_border = 0;
  eng->nk_ctx->style.window.border = 0;
  eng->nk_ctx->style.window.min_row_height_padding = 0;
  eng->nk_ctx->style.window.padding = nk_vec2(0, 0);
  eng->nk_ctx->style.window.combo_padding = nk_vec2(0, 0);
  // Init ecs systems
  eng->ecs_ctx = ecs_init();
  ink_pipeline_init(eng);
  plugin_components_init(eng);
  gui_components_init(eng);
  threed_components_init(eng);
  gui_systems_init(eng);
  threed_systems_init(eng);

  SetTargetFPS(60);
  ecs_set_target_fps(eng->ecs_ctx, 60);
  return eng;
}

/* engine_deinit */
int
engine_deinit(
  engine_t *eng
) {
  CloseWindow();
  nk_raylib_deinit(eng->nk_ctx);
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
  ecs_world_t *ecs_ctx,
  node_id_t node,
  node_id_t *children,
  int max_children
) {
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
 * ENGINE RUNTIME
 */

/* engine_run */
void
engine_run(
  engine_t *eng
) {
  while (!WindowShouldClose()) {
    ecs_progress(eng->ecs_ctx, 0);
  }
}
