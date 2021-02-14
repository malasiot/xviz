set(XVIZ_SERVER_SOURCES 

server.cpp
session.cpp
ws_server.cpp

)

add_definitions(-D ASIO_STANDALONE)

include_directories(
    ../3rdparty/
    ../3rdparty/asio/
     ${ASSIMP_INCLUDE_DIR}
     ${EIGEN3_INCLUDE_DIR}
)

add_library(xviz_server ${LIBRARY_TYPE} ${XVIZ_SERVER_SOURCES})
target_link_libraries(xviz_server xviz)

# Install library
install(TARGETS xviz_server
  EXPORT ${PROJECT_EXPORT}
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT stlib
  COMPONENT dev)
