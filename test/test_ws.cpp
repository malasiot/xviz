#include <xviz/server.hpp>
#include <iostream>
#include <thread>

using namespace std ;

int main() {
    // Create a server endpoint
    xviz::Server server ;
    xviz::Channel *imageChannel = server.createChannel("/data/image", xviz::Channel::IMAGE) ;
    server.createChannel("/data/table", xviz::Channel::TENSOR) ;
    server.createChannel("/data/plot_x", xviz::Channel::TENSOR) ;

    std::thread t([&] {server.run(9002);});

    this_thread::sleep_for(std::chrono::seconds(2));

    server.sendImageUri(imageChannel, "http://image.png");

    t.join() ;
}
