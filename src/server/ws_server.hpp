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

    using connection_t = websocketpp::connection_hdl  ;

    WebSocketServer(Server *controller, const std::string &doc_root);
    WebSocketServer() ;
    ~WebSocketServer();

    void run(int port);
    void send(connection_t con, const std::string &msg) ;
    void broadcast(const std::string &channel, const std::string &msg) ;

private:

    struct BroadcastMessage {
       BroadcastMessage(const std::string &channel, const std::string &msg): channel_(channel), msg_(msg) {}

       std::string channel_ ;
       std::string msg_ ;
    };

    typedef std::map<connection_t, Session ,std::owner_less<connection_t>> ConnectionList ;

private:

    void sendMessageToSubscribers(const std::string &channel, const std::string &msg) ;
    void onOpen(connection_t hdl);

    void onMessage(connection_t hdl, message_ptr msg);

    void onClose(connection_t hdl);

    void onHttp(connection_t hdl);

    void processBroadcastMessages() ;

    Session& getSessionFromConnectionHandle(connection_t hdl);

    std::mutex connections_mutex_, broadcast_mutex_ ;
    std::condition_variable broadcast_cond_ ;
    std::queue<BroadcastMessage> broadcast_messages_;
    int next_session_id_ ;
    server server_;
    ConnectionList connections_;
    Server *controller_ = nullptr ;
    std::string doc_root_ ;
};


}
}









#endif
