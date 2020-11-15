#ifndef XVIZ_SERVER_HPP
#define XVIZ_SERVER_HPP

#include <memory>
#include <map>
#include <string>

#include <xviz/channel.hpp>
#include <xviz/line_chart.hpp>
#include <xviz/bar_chart.hpp>

namespace xviz {

class Image ;

namespace impl {
class Session ;
class WebSocketServer ;
}

class Server {
public:
    Server(const std::string &doc_root = std::string()) ;
    ~Server() ;

    // blocking call to start accepting connections
    void run(int port) ;

    Channel *createChannel(const std::string &name, const Channel::Type ctype) ;
    Channel *findChannel(const std::string &name) ;

    void sendImage(Channel *channel, const xviz::Image &im) ;
    void sendChart(Channel *channel, const Chart &chart) ;

private:

    friend class impl::WebSocketServer ;

    void onSessionStarted(impl::Session &session) ;
    void sendUpdateMessage(Channel *channel, const std::string &msg_data) ;
    void dispatchUpdateMessage(Channel *c, const std::string &msg) ;

    std::map<std::string, Channel> channels_ ;
    std::unique_ptr<impl::WebSocketServer> ws_server_ ;
};


}

#endif
