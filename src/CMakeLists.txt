FIND_PACKAGE(Eigen3 REQUIRED)
FIND_PACKAGE(Assimp REQUIRED)

FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

get_filename_component(LIBRARY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include/clsim/" ABSOLUTE)

include_directories(
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIR}
)

set(LIB_SOURCES
    renderer/gl/gl3w.c
    renderer/shader.cpp
    renderer/material_program.cpp
    renderer/renderer.cpp
    renderer/shadow_map.cpp
    renderer/mesh_data.cpp

    scene/node.cpp
    scene/assimp_loader.cpp
    scene/camera.cpp
    scene/geometry.cpp
    scene/intersect.cpp
    scene/octree.cpp
    scene/raycaster.cpp
    scene/node_helpers.cpp

    common/image.cpp

    gui/qt/viewer.cpp
    ${LIBRARY_INCLUDE_DIR}/gui/qt/viewer.hpp


#    renderer/renderer.cpp
#    renderer/renderer_impl.cpp
#    renderer/mesh_data.cpp
#    renderer/mesh_data.hpp
#    renderer/shadow_map.cpp
#    renderer/shadow_map.hpp
 
#    scene/scene.cpp
#    scene/assimp_loader.cpp
#    scene/camera.cpp
#    scene/mesh.cpp
#    scene/material.cpp

#    scene/texture.cpp
#    scene/geometry.cpp
#    scene/intersect.cpp
#    scene/intersect.hpp
#    scene/octree.hpp
#    scene/octree.cpp
#    scene/marker.cpp

#    ${LIBRARY_INCLUDE_DIR}/scene/node.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/drawable.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/geometry.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/mesh.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/camera.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/light.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/material.hpp
#    ${LIBRARY_INCLUDE_DIR}/scene/marker.hpp

#    ${LIBRARY_INCLUDE_DIR}/renderer/renderer.hpp
)

add_library(clsim SHARED ${LIB_SOURCES})

target_link_libraries(clsim
    ${ASSIMP_LIBRARIES}
    ${OPENGL_LIBRARIES}
    Qt5::Core Qt5::Widgets
)

# Install library
install(TARGETS clsim
  EXPORT ${PROJECT_EXPORT}
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT stlib
  COMPONENT dev)


# Install headers
install(DIRECTORY "${LIBRARY_INCLUDE_DIR}" DESTINATION "${INSTALL_INCLUDE_DIR}/clsim" )
