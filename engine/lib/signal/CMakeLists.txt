cmake_minimum_required(VERSION 3.10)
project(signal)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(srcFiles
		src/signal.c
)

add_library(${PROJECT_NAME} STATIC ${srcFiles})

target_include_directories(${PROJECT_NAME} PRIVATE include)
