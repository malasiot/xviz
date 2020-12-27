add_executable(test_client test_client.cpp )
target_link_libraries(test_client xviz_qt_client)

add_executable(test_scene test_scene.cpp )
target_link_libraries(test_scene xviz_qt_client)

add_executable(test_shapes test_shapes.cpp )
target_link_libraries(test_shapes xviz_qt_client)

add_executable(test_offscreen test_offscreen.cpp )
target_link_libraries(test_offscreen xviz_qt_client)

add_executable(test_picking test_picking.cpp )
target_link_libraries(test_picking xviz_qt_client)

add_executable(test_animation test_animation.cpp )
target_link_libraries(test_animation xviz_qt_client)

add_executable(test_skeleton test_skeleton.cpp )
target_link_libraries(test_skeleton xviz_qt_client)

