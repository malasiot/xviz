file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${PROTO_FILES})
add_library(messages STATIC ${PROTO_SOURCES} ${PROTO_HEADERS})
target_link_libraries(messages ${PROTOBUF_LIBRARY})
