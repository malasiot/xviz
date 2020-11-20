project(xviz)

cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/ ${CMAKE_ROOT}/Modules/)

include(FindThirdParty)

set(XVIZ_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/xviz/)

include_directories(src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${EIGEN3_INCLUDE_DIR}
    )

add_subdirectory(src)
add_subdirectory(test)

