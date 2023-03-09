cmake_minimum_required(VERSION 3.10)
project(inkimera_editor)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_subdirectory(../engine CMakeFiles.engine)

set(srcFiles
  src/main.c
  src/plugins/gui.c
  src/plugins/keyboard_state_machine.c
)

add_executable(${PROJECT_NAME} ${srcFiles})
target_link_libraries(${PROJECT_NAME} inkimera)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ../engine/include)
target_include_directories(${PROJECT_NAME} PRIVATE ../engine/lib/hashmap/include)
