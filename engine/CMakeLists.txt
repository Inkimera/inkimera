cmake_minimum_required(VERSION 3.10)
project(inkimera LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
#set(raylib_VERBOSE 1)

add_subdirectory(lib/signal)
add_subdirectory(lib/raylib_nuklear)
add_subdirectory(external/flecs)
add_subdirectory(external/lua_cmake)
add_subdirectory(external/raylib)

set(srcFiles
		src/engine.c
    src/engine/pipeline.c
    src/engine/components/plugin_components.c
    src/engine/components/gui_components.c
    src/engine/systems/gui_systems.c
    src/engine/components/3d_components.c
    src/engine/systems/3d_systems.c
)

add_library(${PROJECT_NAME} ${srcFiles})

target_link_libraries(${PROJECT_NAME} signal)
target_link_libraries(${PROJECT_NAME} flecs)
target_link_libraries(${PROJECT_NAME} lua::lib)
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} raylib_nuklear)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src/include)
target_include_directories(${PROJECT_NAME} PRIVATE lib/signal/include)
target_include_directories(${PROJECT_NAME} PRIVATE lib/hashmap/include)
target_include_directories(${PROJECT_NAME} PRIVATE lib/raylib_nuklear/include)
target_include_directories(${PROJECT_NAME} PRIVATE external/flecs/include)
target_include_directories(${PROJECT_NAME} PRIVATE external/lua_cmake/include)
target_include_directories(${PROJECT_NAME} PRIVATE external/raygui/src)
target_include_directories(${PROJECT_NAME} PRIVATE external/nuklear)

#target_compile_options(${PROJECT_NAME} INTERFACE
#  -Wall
#  -Wextra
#  -Wpedantic
#  -Werror=all
#)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
