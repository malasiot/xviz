add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(client/qt)

#FIND_PACKAGE(Bullet)

#include_directories(
#    3rdparty
#    3rdparty/asio/
#     ${EIGEN3_INCLUDE_DIR}
#     ${ASSIMP_INCLUDE_DIR}
#)

# Install headers
install(DIRECTORY "${XVIZ_INCLUDE_DIR}" DESTINATION "${INSTALL_INCLUDE_DIR}/xviz" )
