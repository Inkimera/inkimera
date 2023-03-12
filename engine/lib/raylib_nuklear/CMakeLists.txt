cmake_minimum_required(VERSION 3.10)
project(raylib_nuklear)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(srcFiles
		src/raylib_nuklear.c
)

set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
add_library(${PROJECT_NAME} STATIC ${srcFiles})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ../../external/raylib/src)
target_include_directories(${PROJECT_NAME} PRIVATE ../../external/nuklear)
