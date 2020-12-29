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
    lib/chart.cpp
    lib/line_chart.cpp
    lib/bar_chart.cpp
    lib/raster_chart.cpp
    lib/image.cpp
    lib/color.cpp
    lib/pen.cpp
    lib/brush.cpp
    lib/marker.cpp
    lib/annotation.cpp
    lib/xform.cpp
    lib/font.cpp
    lib/colormap.cpp

    lib/path.cpp
    lib/gradient.cpp
    lib/tabular.cpp

    lib/scene/node.cpp
    lib/scene/scene.cpp
    lib/scene/assimp_loader.cpp
    lib/scene/camera.cpp
    lib/scene/material.cpp
    lib/scene/geometry.cpp
    lib/scene/intersect.cpp
    lib/scene/octree.cpp
    lib/scene/raycaster.cpp
    lib/scene/drawable.cpp
    lib/scene/node_helpers.cpp

    lib/robot/urdf_loader.cpp
    lib/robot/urdf_robot.cpp
    lib/robot/robot_scene.cpp

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
    lib/physics/world.cpp
    lib/physics/rigid_body.cpp
    lib/physics/constraints.cpp
    lib/physics/collision.cpp
    lib/physics/multi_body.cpp
    lib/physics/sensor.cpp
    lib/physics/soft_body.cpp

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
