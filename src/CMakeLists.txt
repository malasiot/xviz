add_subdirectory(client/qt)

include_directories(
    ../3rdparty
    ../3rdparty/asio/
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
    types/drawable.cpp
    types/path.cpp
    types/gradient.cpp
    types/tabular.cpp

    ${XVIZ_INCLUDE_DIR}/server.hpp
    ${XVIZ_INCLUDE_DIR}/channel.hpp
    ${XVIZ_INCLUDE_DIR}/line_chart.hpp
    ${XVIZ_INCLUDE_DIR}/bar_chart.hpp
    ${XVIZ_INCLUDE_DIR}/style.hpp
    ${XVIZ_INCLUDE_DIR}/image.hpp
    ${XVIZ_INCLUDE_DIR}/drawable.hpp
    ${XVIZ_INCLUDE_DIR}/path.hpp
    ${XVIZ_INCLUDE_DIR}/gradient.hpp
    ${XVIZ_INCLUDE_DIR}/tabular.hpp
)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR messages/session.proto messages/style.proto messages/drawable.proto)
set_source_files_properties(${PROTO_SRC} ${PROTO_HDR} PROPERTIES GENERATED TRUE)

add_library(xviz ${XVIZ_LIB_SOURCES} ${PROTO_SRC} ${PROTO_HDR})
target_link_libraries(xviz pthread ${PROTOBUF_LIBRARY})
