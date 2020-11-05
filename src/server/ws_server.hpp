#ifndef XVIZ_WS_SERVER_HPP
#define XVIZ_WS_SERVER_HPP

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include "session.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef server::message_ptr message_ptr;

namespace xviz {

class Server ;

namespace impl {

class WebSocketServer {
public:

    WebSocketServer(Server *controller);
    WebSocketServer() ;
    ~WebSocketServer();

    void run(int port);

private:


    typedef std::map<websocketpp::connection_hdl, Session ,std::owner_less<websocketpp::connection_hdl>> ConnectionList ;

private:

    void onOpen(websocketpp::connection_hdl hdl);

    void onMessage(websocketpp::connection_hdl hdl, message_ptr msg);

    void onClose(websocketpp::connection_hdl hdl);

    Session& getSessionFromConnectionHandle(websocketpp::connection_hdl hdl);

    int next_session_id_ ;
    server server_;
    ConnectionList connections_;
    Server *controller_ = nullptr ;
};


}
}









#endif
