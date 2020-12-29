find_package(Bullet)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

IF (BULLET_FOUND) 
include_directories(
    ${XVIZ_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${BULLET_INCLUDE_DIRS}
)

ADD_DEFINITIONS(-DBT_USE_DOUBLE_PRECISION=1)

add_executable(test_bullet test_bullet.cpp bullet_gui.cpp bullet_gui.hpp body.cpp body.hpp)
target_link_libraries(test_bullet xviz_qt_client ${BULLET_LIBRARIES})

add_executable(test_rope test_rope.cpp bullet_gui.cpp bullet_gui.hpp)
set_target_properties(test_rope PROPERTIES AUTOMOC TRUE)
target_link_libraries(test_rope xviz_qt_client ${BULLET_LIBRARIES})

add_executable(test_gripper test_gripper.cpp bullet_gui.cpp bullet_gui.hpp)
set_target_properties(test_gripper PROPERTIES AUTOMOC TRUE)
target_link_libraries(test_gripper xviz_qt_client ${BULLET_LIBRARIES})

add_executable(test_cartpole test_cartpole.cpp bullet_gui.cpp bullet_gui.hpp)
set_target_properties(test_cartpole PROPERTIES AUTOMOC TRUE)
target_link_libraries(test_cartpole xviz_qt_client ${BULLET_LIBRARIES})

add_executable(test_bullet_robot test_robot.cpp bullet_gui.cpp bullet_gui.hpp)
set_target_properties(test_bullet_robot PROPERTIES AUTOMOC TRUE)
target_link_libraries(test_bullet_robot xviz_qt_client ${BULLET_LIBRARIES})

add_executable(test_cloth test_cloth.cpp bullet_gui.cpp bullet_gui.hpp)
set_target_properties(test_cloth PROPERTIES AUTOMOC TRUE)
target_link_libraries(test_cloth xviz_qt_client ${BULLET_LIBRARIES})

ENDIF(BULLET_FOUND)
