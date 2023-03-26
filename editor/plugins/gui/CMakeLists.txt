cmake_minimum_required(VERSION 3.10)
project(inkimera_editor_gui LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(srcFiles
  src/gui.c
  src/gui_node.c
  src/keyboard_state_machine.c
  src/systems/gui_input.c
  src/systems/gui_update.c
  src/widgets/gui_scene_widget.c
  src/widgets/gui_scene_graph_widget.c
  src/widgets/gui_menu_header_widget.c
  src/widgets/gui_menu/gui_new_project_widget.c
  src/widgets/gui_menu/gui_open_project_widget.c
)

add_library(${PROJECT_NAME} STATIC ${srcFiles})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../lib/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/lib/hashmap/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/lib/raylib_nuklear/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/flecs/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/lua_cmake/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/nuklear)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/raylib/src)
target_compile_options(${PROJECT_NAME} INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -Werror=all
)
