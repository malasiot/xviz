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

    void sendRawImageData(Channel *channel, uint32_t width, uint32_t height, uint32_t stride, int pixel_type, char *bytes);
    void sendImageUri(Channel *channel, const std::string &uri) ;

private:

    friend class impl::WebSocketServer ;

    void onSessionStarted(impl::Session &session) ;
    void dispatchUpdateMessage(Channel *c, const std::string &msg) ;

    std::map<std::string, Channel> channels_ ;

    std::unique_ptr<impl::WebSocketServer> ws_server_ ;
};


}

#endif
