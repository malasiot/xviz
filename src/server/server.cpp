#include <xviz/server.hpp>
#include "ws_server.hpp"
#include "session.hpp"

using namespace std ;

namespace xviz {

Server::Server(): ws_server_(new impl::WebSocketServer(this)) {

}

Server::~Server() = default ;

void Server::run(int port) {
    ws_server_->run(port) ;
}

Channel *Server::createChannel(const string &name, const Channel::Type ctype) {
    auto res = channels_.emplace(std::make_pair(name, Channel{name, ctype})) ;
    if ( res.second ) return &((*(res.first)).second) ;
    else return nullptr ;
}

Channel *Server::findChannel(const string &name) {
    auto res = channels_.find(name) ;
    if ( res != channels_.end() ) return &((*res).second) ;
    else return nullptr ;
}


void Server::onSessionStarted(impl::Session &session)
{

}



}
