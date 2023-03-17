#ifndef INKIMERA_GUI_H
#define INKIMERA_GUI_H

#include "raylib_nuklear.h"

/*
 * CORE
 */
/* GuiRoot */
extern ECS_TAG_DECLARE(GuiRoot);

/* GuiFocus */
extern ECS_TAG_DECLARE(GuiFocus);

/* gui_nk_context_t */
typedef struct {
  struct nk_context *nk_ctx;
} gui_nk_context_t;
extern ECS_COMPONENT_DECLARE(gui_nk_context_t);
#endif
