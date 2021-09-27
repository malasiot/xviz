add_executable(test_renderer renderer.cpp )
target_link_libraries(test_renderer xviz)

add_executable(test_animation animation.cpp )
target_link_libraries(test_animation xviz)

add_executable(test_offscreen offscreen.cpp )
target_link_libraries(test_offscreen xviz)

add_executable(test_picking picking.cpp )
target_link_libraries(test_picking xviz)

add_executable(test_robot robot.cpp )
target_link_libraries(test_robot xviz)

add_executable(test_skeleton skeleton.cpp )
target_link_libraries(test_skeleton xviz)

add_executable(test_shadows shadows.cpp )
target_link_libraries(test_shadows xviz)

SET(PHYSICS_SRC
    physics/particle.cpp
    physics/cloth.cpp
    physics/constraint.cpp
    physics/solver.cpp
    physics/world.cpp
    physics/spatial_hashing.cpp
    physics/collision_object.cpp
)


add_executable(test_cloth test_cloth.cpp sim_gui.cpp ${PHYSICS_SRC})
target_link_libraries(test_cloth xviz)

add_executable(test_manipulators manipulators.cpp )
target_link_libraries(test_manipulators xviz)
