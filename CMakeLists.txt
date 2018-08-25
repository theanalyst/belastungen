cmake_minimum_required(VERSION 3.2)
include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/ruslo/hunter/archive/v0.23.17.tar.gz"
  SHA1 "a438452aa14fc5cbd58f080eee45f07351b27d2f"
  LOCAL
)

project(belastungen)
hunter_add_package(Boost COMPONENTS system)
find_package(Boost CONFIG REQUIRED system)
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -lpthread")
add_subdirectory(src)
