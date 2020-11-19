add_subdirectory(client/qt)

include_directories(
    ../3rdparty
    ../3rdparty/asio/
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
    types/chart.cpp
    types/line_chart.cpp
    types/bar_chart.cpp
    types/image.cpp
    types/color.cpp
    types/pen.cpp
    types/brush.cpp
    types/marker.cpp

    types/path.cpp
    types/gradient.cpp
    types/tabular.cpp

    types/scene/node.cpp
    types/scene/scene.cpp
    types/scene/mesh.cpp
    types/scene/assimp_loader.cpp

    ${XVIZ_INCLUDE_DIR}/server.hpp
    ${XVIZ_INCLUDE_DIR}/channel.hpp
    ${XVIZ_INCLUDE_DIR}/line_chart.hpp
    ${XVIZ_INCLUDE_DIR}/bar_chart.hpp
    ${XVIZ_INCLUDE_DIR}/style.hpp
    ${XVIZ_INCLUDE_DIR}/image.hpp
    ${XVIZ_INCLUDE_DIR}/path.hpp
    ${XVIZ_INCLUDE_DIR}/gradient.hpp
    ${XVIZ_INCLUDE_DIR}/tabular.hpp

    ${XVIZ_INCLUDE_DIR}/scene/node.hpp
    ${XVIZ_INCLUDE_DIR}/scene/scene.hpp
    ${XVIZ_INCLUDE_DIR}/scene/scene_fwd.hpp
    ${XVIZ_INCLUDE_DIR}/scene/mesh.hpp
    ${XVIZ_INCLUDE_DIR}/scene/drawable.hpp
    ${XVIZ_INCLUDE_DIR}/scene/material.hpp
)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR messages/session.proto messages/style.proto messages/drawable.proto)
set_source_files_properties(${PROTO_SRC} ${PROTO_HDR} PROPERTIES GENERATED TRUE)

add_library(xviz ${XVIZ_LIB_SOURCES} ${PROTO_SRC} ${PROTO_HDR})
target_link_libraries(xviz pthread ${PROTOBUF_LIBRARY} ${ASSIMP_LIBRARIES})
