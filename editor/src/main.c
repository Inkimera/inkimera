#include "stdlib.h"

#include "inkimera.h"
#include "gui.h"
#include "scene_editor.h"

/*
 * MAIN
 */
int
main(
  void
) {
  // INIT
  engine_t *eng = engine_init();
  // Plugins
  gui_plugin_init(eng);
  scene_editor_plugin_init(eng);
  // RUN
  engine_run(eng);
  // DEINIT
  engine_deinit(eng);
  return 0;
}
