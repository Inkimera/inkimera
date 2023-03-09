#include "inkimera.h"
#include "plugins/gui.h"

/*
 * MAIN
 */
int
main(
  void
) {
  engine_t *eng = engine_init();

  gui_plugin_t *gui_plugin = gui_plugin_init();
  plugin_handle_t *gui_plugin_handle
    = engine_create_plugin(eng, gui_plugin, &gui_plugin_load, &gui_plugin_unload, &gui_plugin_events);
  engine_register_plugin(eng, gui_plugin_handle);

  engine_run(eng);
  engine_deinit(eng);
  return 0;
}
