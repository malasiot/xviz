#include "ws_server.hpp"

#include <xviz/server.hpp>

#include <session.pb.h>

using namespace std ;

namespace xviz { namespace impl {


WebSocketServer::WebSocketServer(Server *ctrl): controller_(ctrl) {

    try {
        // Set logging settings
        server_.set_access_channels(websocketpp::log::alevel::all);
        server_.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        server_.init_asio();

        // Register our message handler
        //  echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));
        server_.set_message_handler(std::bind(&WebSocketServer::onMessage, this, ::_1, ::_2)) ;
        server_.set_http_handler([&](websocketpp::connection_hdl hdl){
            server::connection_ptr con = server_.get_con_from_hdl(hdl);
            std::string path = con->get_resource();
            cout << path << endl ;

        });

        server_.set_open_handler(std::bind(&WebSocketServer::onOpen, this, ::_1)) ;
        server_.set_close_handler(std::bind(&WebSocketServer::onClose, this, ::_1)) ;


    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

WebSocketServer::WebSocketServer()
{

}

WebSocketServer::~WebSocketServer() {} ;


void WebSocketServer::run(int port) {

    thread t(std::bind(&WebSocketServer::processBroadcastMessages,this));
    // Listen on port 9002
    server_.listen(asio::ip::tcp::v4(), port);

    // Start the server accept loop
    server_.start_accept();

    // Start the ASIO io_service run loop
    server_.run();

    t.join() ;
}

void WebSocketServer::broadcast(const string &channel, const string &msg) {
    {
        lock_guard<mutex> guard(broadcast_mutex_);
        //std::cout << "on_message" << std::endl;
        broadcast_messages_.push(BroadcastMessage(channel, msg) );
    }
    broadcast_cond_.notify_one();
}

void WebSocketServer::sendMessageToSubscribers(const string &channel, const string &msg)
{
    std::lock_guard<std::mutex> guard(connections_mutex_);

    for( const auto &cp: connections_ ) {
        const connection_t &con = cp.first ;
        const Session &session = cp.second ;
        for ( const string &c: session.channels_ ) {
            if ( c == channel )
                send(con, msg) ;
        }
    }
}

void WebSocketServer::processBroadcastMessages()
{
    while(1) {
        unique_lock<mutex> lock(broadcast_mutex_);

        while(broadcast_messages_.empty()) {
            broadcast_cond_.wait(lock);
        }

        BroadcastMessage m = broadcast_messages_.front();
        broadcast_messages_.pop();

        cout << m.channel_ << endl ;

        lock.unlock();

        sendMessageToSubscribers(m.channel_, m.msg_);
    }


}

void WebSocketServer::send(websocketpp::connection_hdl hdl, const string &msg) {
    server_.send(hdl, msg, websocketpp::frame::opcode::BINARY);
}

void WebSocketServer::onOpen(websocketpp::connection_hdl hdl) {
    Session data;

    std::lock_guard<std::mutex> guard(connections_mutex_);

    data.id_ = next_session_id_ ++;
    data.connection_ = hdl ;
    connections_[hdl] = data;
}

void WebSocketServer::onMessage(websocketpp::connection_hdl hdl, message_ptr msg) {

    Session &session = getSessionFromConnectionHandle(hdl) ;

    auto payload = msg->get_payload() ;

    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << payload
              << std::endl;

    try {
        msg::Message msg_body ;
        if ( msg_body.ParseFromString(payload) ) {
            switch ( msg_body.msg_case() ) {
            case msg::Message::kSessionStart: {
                const msg::SessionStart &session_start_msg = msg_body.session_start() ;
                string session_name = session_start_msg.name() ;
                auto &channels = session_start_msg.channels() ;
                vector<string> channel_list(channels.begin(), channels.end()) ;

                session.name_ = session_name ;
                session.channels_ = channel_list ;

                controller_->onSessionStarted(session) ;
                break ;
            }
            }
        }

        //server_.send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                  << "(" << e.what() << ")" << std::endl;
    }
}

void WebSocketServer::onClose(websocketpp::connection_hdl hdl) {
    Session &data = getSessionFromConnectionHandle(hdl) ;
    connections_.erase(hdl);
}

Session &WebSocketServer::getSessionFromConnectionHandle(websocketpp::connection_hdl hdl) {
    std::lock_guard<std::mutex> guard(connections_mutex_);

    auto it = connections_.find(hdl);

    if (it == connections_.end()) {
        // this connection is not in the list. This really shouldn't happen
        // and probably means something else is wrong.
        throw std::invalid_argument("No data available for session");
    }

    return it->second;
}

} // namespace impl
               } // namespace xviz
