#include <xviz/server.hpp>
#include <xviz/image.hpp>
#include <iostream>
#include <thread>

using namespace std ;

int main() {
    // Create a server endpoint
    xviz::Server server ;

    std::thread t([&] {server.run(9002);});

    this_thread::sleep_for(std::chrono::seconds(2));

    xviz::Image im("http://image.png") ;
    server.push("/data/image", im);

    t.join() ;
}
