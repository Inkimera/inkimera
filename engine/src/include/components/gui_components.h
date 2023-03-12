#ifndef GUI_COMPONENTS_H
#define GUI_COMPONENTS_H
#include "engine.h"
#include "raylib_nuklear.h"

/* gui_nk_context_t */
typedef struct {
  struct nk_context *nk_ctx;
} gui_nk_context_t;
extern ECS_COMPONENT_DECLARE(gui_nk_context_t);

/* gui_components_init*/
int
gui_components_init(
  engine_t *eng
);
#endif
