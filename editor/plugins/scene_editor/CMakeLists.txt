cmake_minimum_required(VERSION 3.10)
project(inkimera_editor_scene_editor LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(srcFiles
  src/scene_editor.c
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
