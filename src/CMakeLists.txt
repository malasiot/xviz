add_subdirectory(client/qt)


FIND_PACKAGE(Bullet)

include_directories(
    3rdparty
    3rdparty/asio/
     ${EIGEN3_INCLUDE_DIR}
     ${ASSIMP_INCLUDE_DIR}
)

set(XVIZ_LIB_SOURCES
    util/json_reader.cpp
    util/json_reader.hpp
    server/server.cpp
    server/ws_server.cpp
    server/ws_server.hpp
    server/session.cpp
    server/session.hpp
    server/channel.cpp
    core/chart.cpp
    core/line_chart.cpp
    core/bar_chart.cpp
    core/raster_chart.cpp
    core/image.cpp
    core/color.cpp
    core/pen.cpp
    core/brush.cpp
    core/marker.cpp
    core/annotation.cpp
    core/xform.cpp
    core/font.cpp
    core/colormap.cpp

    core/path.cpp
    core/gradient.cpp
    core/tabular.cpp

    core/scene/node.cpp
    core/scene/scene.cpp
    core/scene/assimp_loader.cpp
    core/scene/camera.cpp
    core/scene/material.cpp
    core/scene/geometry.cpp
    core/scene/intersect.cpp
    core/scene/octree.cpp
    core/scene/raycaster.cpp
    core/scene/drawable.cpp
    core/scene/node_helpers.cpp

    core/robot/urdf_loader.cpp
    core/robot/urdf_robot.cpp
    core/robot/robot_scene.cpp

    3rdparty/pugi/pugixml.cpp

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
INCLUDE_DIRECTORIES(${BULLET_INCLUDE_DIRS})
LIST(APPEND XVIZ_LIB_SOURCES
    core/physics/world.cpp
    core/physics/rigid_body.cpp
    core/physics/constraints.cpp
    core/physics/collision.cpp
    core/physics/multi_body.cpp
    core/physics/sensor.cpp
    core/physics/soft_body.cpp

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

add_library(xviz ${XVIZ_LIB_SOURCES} ${PROTO_SRC} ${PROTO_HDR})
target_link_libraries(xviz pthread ${PROTOBUF_LIBRARY} ${ASSIMP_LIBRARIES} ${BULLET_LIBRARIES})
