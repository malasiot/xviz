include_directories(
    ../3rdparty
    ../3rdparty/asio/
)

add_executable(test_ws test_ws.cpp)
target_link_libraries(test_ws pthread)

