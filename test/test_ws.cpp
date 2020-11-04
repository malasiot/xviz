#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <rapidjson/document.h>

#include <iostream>

#include "channel.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

using namespace std ;
using namespace rapidjson;

class Server {
public:

    Server() {

        try {
            // Set logging settings
            server_.set_access_channels(websocketpp::log::alevel::all);
            server_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            // Initialize Asio
            server_.init_asio();

            // Register our message handler
          //  echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));
              server_.set_message_handler(std::bind(&Server::onMessage, this, ::_1, ::_2)) ;
              server_.set_http_handler([&](websocketpp::connection_hdl hdl){
                  server::connection_ptr con = server_.get_con_from_hdl(hdl);
                  std::string path = con->get_resource();
                    cout << path << endl ;

                  });

              server_.set_open_handler(std::bind(&Server::onOpen, this, ::_1)) ;
              server_.set_close_handler(std::bind(&Server::onClose, this, ::_1)) ;


        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
        } catch (...) {
            std::cout << "other exception" << std::endl;
        }
    }

    void run(int port) {

        // Listen on port 9002
        server_.listen(asio::ip::tcp::v4(), port);

        // Start the server accept loop
        server_.start_accept();

        // Start the ASIO io_service run loop
        server_.run();
    }

protected:

    void onOpen(websocketpp::connection_hdl hdl) {

        SessionData data;

        data.id_ = next_session_id_ ++;
        connections_[hdl] = data;
    }

    // Define a callback to handle incoming messages
    void onMessage(websocketpp::connection_hdl hdl, message_ptr msg) {

        SessionData &session = getSessionFromConnectionHandle(hdl) ;

        auto payload = msg->get_payload() ;

        std::cout << "on_message called with hdl: " << hdl.lock().get()
                  << " and message: " << payload
                  << std::endl;

        try {
            Document doc ;
            doc.Parse(payload.c_str()) ;

            if ( doc["message"] == "session" ) {
                const auto &channelArray = doc["channels"];
                session.channels_.clear() ;
                for ( int i=0 ; i<channelArray.Size(); ++i ) {
                     string channel = channelArray[i].GetString();
                    session.channels_.push_back(channel) ;
                }
            }

            server_.send(hdl, msg->get_payload(), msg->get_opcode());
        } catch (websocketpp::exception const & e) {
            std::cout << "Echo failed because: "
                      << "(" << e.what() << ")" << std::endl;
        }
    }

    void onClose(websocketpp::connection_hdl hdl) {
      SessionData &data = getSessionFromConnectionHandle(hdl) ;
      connections_.erase(hdl);
    }

    struct SessionData {
        uint64_t id_ ;
        std::vector<std::string> channels_ ;
    };

    SessionData& getSessionFromConnectionHandle(websocketpp::connection_hdl hdl) {
         auto it = connections_.find(hdl);

         if (it == connections_.end()) {
             // this connection is not in the list. This really shouldn't happen
             // and probably means something else is wrong.
             throw std::invalid_argument("No data available for session");
         }

         return it->second;
     }


    typedef std::map<websocketpp::connection_hdl, SessionData ,std::owner_less<websocketpp::connection_hdl>> ConnectionList ;

    int next_session_id_ ;
    server server_;
    ConnectionList connections_;
    std::vector<Channel> channels_ ;

};





int main() {
    // Create a server endpoint
    Server server ;
    server.run(9002);
}
