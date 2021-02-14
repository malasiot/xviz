
FIND_PACKAGE(Bullet)

include_directories(
    ../3rdparty
     ${ASSIMP_INCLUDE_DIR}
     ${EIGEN3_INCLUDE_DIR}
)

set(XVIZ_LIB_SOURCES
    chart.cpp
    line_chart.cpp
    bar_chart.cpp
    raster_chart.cpp
    image.cpp
    color.cpp
    pen.cpp
    brush.cpp
    marker.cpp
    annotation.cpp
    xform.cpp
    font.cpp
    colormap.cpp

    path.cpp
    gradient.cpp
    tabular.cpp

    scene/node.cpp
    scene/scene.cpp
    scene/assimp_loader.cpp
    scene/camera.cpp
    scene/material.cpp
    scene/geometry.cpp
    scene/intersect.cpp
    scene/octree.cpp
    scene/raycaster.cpp
    scene/drawable.cpp
    scene/node_helpers.cpp

    robot/urdf_loader.cpp
    robot/urdf_robot.cpp
    robot/robot_scene.cpp

    ../3rdparty/pugi/pugixml.cpp

    ${XVIZ_INCLUDE_DIR}/server.hpp
    ${XVIZ_INCLUDE_DIR}/channel.hpp
    ${XVIZ_INCLUDE_DIR}/line_chart.hpp
    ${XVIZ_INCLUDE_DIR}/bar_chart.hpp
    ${XVIZ_INCLUDE_DIR}/raster_chart.hpp
    ${XVIZ_INCLUDE_DIR}/style.hpp
    ${XVIZ_INCLUDE_DIR}/image.hpp
    ${XVIZ_INCLUDE_DIR}/path.hpp
    ${XVIZ_INCLUDE_DIR}/colormap.hpp
    ${XVIZ_INCLUDE_DIR}/gradient.hpp
    ${XVIZ_INCLUDE_DIR}/tabular.hpp

    ${XVIZ_INCLUDE_DIR}/scene/node.hpp
    ${XVIZ_INCLUDE_DIR}/scene/scene.hpp
    ${XVIZ_INCLUDE_DIR}/scene/scene_fwd.hpp
    ${XVIZ_INCLUDE_DIR}/scene/drawable.hpp
    ${XVIZ_INCLUDE_DIR}/scene/material.hpp
    ${XVIZ_INCLUDE_DIR}/scene/camera.hpp
    ${XVIZ_INCLUDE_DIR}/scene/geometry.hpp
    ${XVIZ_INCLUDE_DIR}/scene/raycaster.hpp
    ${XVIZ_INCLUDE_DIR}/scene/detail/octree.hpp
    ${XVIZ_INCLUDE_DIR}/scene/detail/intersect.hpp
)

IF ( BULLET_FOUND )
ADD_DEFINITIONS(-DBT_USE_DOUBLE_PRECISION=1)
include_directories(${BULLET_INCLUDE_DIRS})
LIST(APPEND XVIZ_LIB_SOURCES
    physics/world.cpp
    physics/rigid_body.cpp
    physics/constraints.cpp
    physics/collision.cpp
    physics/multi_body.cpp
    physics/sensor.cpp
    physics/soft_body.cpp

    ${XVIZ_INCLUDE_DIR}/physics/world.hpp
    ${XVIZ_INCLUDE_DIR}/physics/rigid_body.hpp
    ${XVIZ_INCLUDE_DIR}/physics/collision.hpp
    ${XVIZ_INCLUDE_DIR}/physics/convert.hpp
    ${XVIZ_INCLUDE_DIR}/physics/constraints.hpp
    ${XVIZ_INCLUDE_DIR}/physics/multi_body.hpp
    ${XVIZ_INCLUDE_DIR}/physics/sensor.hpp
    ${XVIZ_INCLUDE_DIR}/physics/soft_body.hpp
)
ENDIF( BULLET_FOUND )

protobuf_generate_cpp(PROTO_SRC PROTO_HDR messages/core.proto messages/session.proto messages/style.proto messages/drawable.proto messages/scene.proto)
set_source_files_properties(${PROTO_SRC} ${PROTO_HDR} PROPERTIES GENERATED TRUE)

add_library(xviz ${LIBRARY_TYPE} ${XVIZ_LIB_SOURCES} ${PROTO_SRC} ${PROTO_HDR})
target_link_libraries(xviz pthread ${PROTOBUF_LIBRARY} ${ASSIMP_LIBRARIES} ${BULLET_LIBRARIES})

# Install library
install(TARGETS xviz
  EXPORT ${PROJECT_EXPORT}
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT stlib
  COMPONENT dev)
