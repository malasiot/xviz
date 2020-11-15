#include <xviz/server.hpp>
#include <xviz/image.hpp>
#include <iostream>
#include <thread>

using namespace std ;

int main() {
    // Create a server endpoint
    xviz::Server server ;
    xviz::Channel *imageChannel = server.createChannel("/data/image", xviz::Channel::IMAGE) ;
    server.createChannel("/data/table", xviz::Channel::TABLE) ;
    server.createChannel("/data/plot", xviz::Channel::CHART) ;

    std::thread t([&] {server.run(9002);});

    this_thread::sleep_for(std::chrono::seconds(2));

    xviz::ImageUri im("http://image.png") ;
    server.sendImage(imageChannel, im);

    t.join() ;
}
