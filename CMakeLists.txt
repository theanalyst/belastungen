cmake_minimum_required(VERSION 3.2)
include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/ruslo/hunter/archive/v0.23.17.tar.gz"
  SHA1 "a438452aa14fc5cbd58f080eee45f07351b27d2f"
  LOCAL
)

project(belastungen)
hunter_add_package(Boost COMPONENTS system coroutine context thread)
find_package(Boost CONFIG REQUIRED system coroutine context thread)
find_program(CCACHE_FOUND "ccache")
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
find_package(Threads REQUIRED)
add_subdirectory(src)
