add_executable(test_client test_client.cpp )
target_link_libraries(test_client xviz_qt_client)

add_executable(test_scene test_scene.cpp )
target_link_libraries(test_scene xviz_qt_client)

