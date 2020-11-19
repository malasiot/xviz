#ifndef XVIZ_SERVER_HPP
#define XVIZ_SERVER_HPP

#include <memory>
#include <map>
#include <string>

#include <xviz/channel.hpp>
#include <xviz/line_chart.hpp>
#include <xviz/bar_chart.hpp>
#include <xviz/tabular.hpp>

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

    void push(const std::string &channel, const xviz::Image &im) ;
    void push(const std::string &channel, const Chart &chart) ;
    void push(const std::string &channel, const Tabular &tab);

private:

    std::string version() { return "1.0" ; }

    friend class impl::WebSocketServer ;

    void onSessionStarted(impl::Session &session) ;
    void sendUpdateMessage(const std::string &channel, const std::string &msg_data) ;
    void dispatchUpdateMessage(const std::string &channel, const std::string &msg) ;

    std::unique_ptr<impl::WebSocketServer> ws_server_ ;
};


}

#endif
