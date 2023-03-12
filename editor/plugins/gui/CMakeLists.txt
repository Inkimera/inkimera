cmake_minimum_required(VERSION 3.10)
project(inkimera_editor_gui LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(srcFiles
  src/gui.c
  src/keyboard_state_machine.c
)

add_library(${PROJECT_NAME} ${srcFiles})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/lib/hashmap/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/flecs/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/raylib/src)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../engine/external/lua_cmake/include)
target_compile_options(${PROJECT_NAME} INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -Werror=all
)
