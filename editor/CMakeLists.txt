cmake_minimum_required(VERSION 3.10)
project(inkimera_editor LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory(plugins/gui)
add_subdirectory(plugins/scene_editor)
add_subdirectory(../engine CMakeFiles.engine)

set(srcFiles
  src/main.c
)

add_executable(${PROJECT_NAME} ${srcFiles})
target_link_libraries(${PROJECT_NAME} inkimera_editor_gui)
target_link_libraries(${PROJECT_NAME} inkimera_editor_scene_editor)
target_link_libraries(${PROJECT_NAME} inkimera)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE plugins/gui/include)
target_include_directories(${PROJECT_NAME} PRIVATE plugins/scene_editor/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../engine/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../engine/lib/hashmap/include)
target_compile_options(${PROJECT_NAME} INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -Werror=all
)
