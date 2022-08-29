
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(test_renderer util.cpp renderer.cpp )
target_link_libraries(test_renderer xviz)

add_executable(test_animation util.cpp animation.cpp )
target_link_libraries(test_animation xviz)

add_executable(test_offscreen util.cpp offscreen.cpp )
target_link_libraries(test_offscreen xviz)

add_executable(test_picking util.cpp picking.cpp )
target_link_libraries(test_picking xviz)

add_executable(test_robot util.cpp robot.cpp )
target_link_libraries(test_robot xviz)

add_executable(test_skeleton util.cpp skeleton.cpp )
target_link_libraries(test_skeleton xviz)

add_executable(test_shadows util.cpp shadows.cpp )
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


add_executable(test_cloth util.cpp test_cloth.cpp sim_gui.cpp ${PHYSICS_SRC})
target_link_libraries(test_cloth xviz)

add_executable(test_manipulators util.cpp manipulators.cpp )
target_link_libraries(test_manipulators xviz)

add_subdirectory(mh)

