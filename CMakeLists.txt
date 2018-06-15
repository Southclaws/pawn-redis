cmake_minimum_required(VERSION 3.0)

project(pawn-redis)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/cmake-modules")

if(UNIX)
	set(CMAKE_CXX_FLAGS "-m32 -fvisibility=hidden -std=c++14")
	set(CMAKE_C_FLAGS "-m32 -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
else()
	set(MSVC_RUNTIME_LIBRARY_CONFIG "/MD")
endif()

# removes the sprintf warnings from plugin-natives
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/test/plugins)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/test/plugins)
set(CMAKE_BUILD_TYPE Release)
set(BUILD_SHARED_LIBS 0)
set(LOGGING_ENABLED OFF)

set(BUILD_EXAMPLES OFF)
set(BUILD_TESTS OFF)

include_directories(lib/cpp_redis/tacopie/includes)
add_subdirectory(lib/cpp_redis)
include_directories(lib/cpp_redis/includes)

add_subdirectory(src)
