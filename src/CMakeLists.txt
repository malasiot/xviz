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

    ${XVIZ_INCLUDE_DIR}/server.hpp
    ${XVIZ_INCLUDE_DIR}/channel.hpp
)

protobuf_generate_cpp(PROTO_SRC PROTO_HDR messages/session.proto)
set_source_files_properties(${PROTO_SRC} ${PROTO_HDR} PROPERTIES GENERATED TRUE)

add_library(xviz ${XVIZ_LIB_SOURCES} ${PROTO_SRC} ${PROTO_HDR})
target_link_libraries(xviz pthread ${PROTOBUF_LIBRARY})
