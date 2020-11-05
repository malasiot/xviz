#ifndef XVIZ_SERVER_HPP
#define XVIZ_SERVER_HPP

#include <memory>
#include <map>
#include <string>

#include <xviz/channel.hpp>

namespace xviz {


namespace impl {
class Session ;
class WebSocketServer ;
}

class Server {
public:
    Server() ;
    ~Server() ;

    // blocking call to start accepting connections
    void run(int port) ;

    Channel *createChannel(const std::string &name, const Channel::Type ctype) ;
    Channel *findChannel(const std::string &name) ;

private:

    friend class impl::WebSocketServer ;

    void onSessionStarted(impl::Session &session) ;

    std::map<std::string, Channel> channels_ ;

    std::unique_ptr<impl::WebSocketServer> ws_server_ ;
};


}

#endif
