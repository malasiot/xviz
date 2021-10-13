
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


include_directories(
    3rdparty/
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
    renderer/texture_data.cpp
    renderer/resource_manager.cpp

    scene/node.cpp
    scene/assimp_loader.cpp
    scene/camera.cpp
    scene/geometry.cpp
    scene/intersect.cpp
    scene/octree.cpp
    scene/raycaster.cpp
    scene/node_helpers.cpp

    robot/robot_scene.cpp
    robot/urdf_loader.cpp
    robot/urdf_robot.cpp

    common/image.cpp

    3rdparty/pugi/pugixml.cpp

    gui/viewer.cpp
    gui/trackball.cpp
    gui/offscreen.cpp
    gui/manipulator.cpp
    gui/translation_manipulator.cpp
    gui/rotation_manipulator.cpp

    ${LIBRARY_INCLUDE_DIR}/gui/viewer.hpp
    ${LIBRARY_INCLUDE_DIR}/gui/offscreen.hpp
)

add_library(xviz SHARED ${LIB_SOURCES})

target_link_libraries(xviz
    ${ASSIMP_LIBRARIES}
    ${OPENGL_LIBRARIES}
    Qt5::Core Qt5::Widgets
)

# Install library
install(TARGETS xviz
  EXPORT ${PROJECT_EXPORT}
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT stlib
  COMPONENT dev)


# Install headers
install(DIRECTORY "${LIBRARY_INCLUDE_DIR}" DESTINATION "${INSTALL_INCLUDE_DIR}/xviz" )
