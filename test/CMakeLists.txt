
add_executable(test_ws test_ws.cpp )
target_link_libraries(test_ws pthread xviz xviz_server)

add_subdirectory(qt)
add_subdirectory(physics)

