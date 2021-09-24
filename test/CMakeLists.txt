add_executable(test_renderer renderer.cpp )
target_link_libraries(test_renderer xviz)

add_executable(test_cloth test_cloth.cpp sim_gui.cpp )
target_link_libraries(test_cloth xviz)

add_executable(test_animation animation.cpp )
target_link_libraries(test_animation xviz)

