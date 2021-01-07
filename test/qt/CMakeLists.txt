
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

add_executable(test_robot test_robot.cpp )
target_link_libraries(test_robot xviz_qt_client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Widgets WebSockets Xml REQUIRED)

QT5_ADD_RESOURCES(TEST_IMAGEVIEW_RC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/image_view.qrc)
set_source_files_properties(${TEST_IMAGEVIEW_RC_SOURCES} PROPERTIES GENERATED ON)
message(${TEST_IMAGEVIEW_RC_SOURCES})
add_executable(test_image_view test_image_view.cpp image_view.cpp image_view.hpp ${TEST_IMAGEVIEW_RC_SOURCES})
target_link_libraries(test_image_view xviz_qt_client)
