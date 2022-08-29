find_package(cvx REQUIRED)
add_executable(test_mh main.cpp bvh.cpp mhx2_importer.cpp mhx2_viz_scene.cpp ../util.cpp)
target_link_libraries(test_mh xviz cvx)
