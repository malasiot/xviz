project(xviz)

cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/ ${CMAKE_ROOT}/Modules/)

include(FindThirdParty)

include_directories(src)

add_subdirectory(src)
add_subdirectory(test)

