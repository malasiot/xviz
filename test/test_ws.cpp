#include <xviz/server.hpp>
#include <iostream>

using namespace std ;

int main() {
    // Create a server endpoint
    xviz::Server server ;
    server.run(9002);
}
