find_package(cvx REQUIRED)
find_package(OpenCV 4 REQUIRED COMPONENTS core imgproc highgui imgcodecs)

INCLUDE_DIRECTORIES( ${OpenCV_INCLUDE_DIRS} )
add_executable(make_texture make_texture.cpp mhx2_importer.cpp)
target_link_libraries(make_texture cvx ${OpenCV_LIBS})

add_executable(test_mh main.cpp mhx2_importer.cpp mhx2_viz_scene.cpp ../util.cpp)
target_link_libraries(test_mh cvx xviz)
