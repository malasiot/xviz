#include <xviz/server.hpp>
#include "ws_server.hpp"
#include "session.hpp"
#include "session.pb.h"
#include <xviz/image.hpp>

using namespace std ;

namespace xviz {

Server::Server(const string &doc_root): ws_server_(new impl::WebSocketServer(this, doc_root)) {

}

Server::~Server() {}

void Server::run(int port) {
    ws_server_->run(port) ;
}

/*
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
*/
void Server::sendUpdateMessage(const std::string &channel, const string &data) {
    msg::StateUpdate *update = new msg::StateUpdate() ;
    update->set_ts(0.0);
    update->set_channel_id(channel) ;
    update->set_data(data) ;

    msg::Message msg ;
    msg.set_allocated_state_update(update);
    dispatchUpdateMessage(channel, msg.SerializeAsString());
}

void Server::push(const std::string &channel, const Image &im) {
    std::unique_ptr<msg::Image> im_msg(Image::write(im)) ;
    sendUpdateMessage(channel, im_msg->SerializeAsString());
}

void Server::push(const std::string &channel, const Chart &chart) {
    sendUpdateMessage(channel, Chart::write(&chart)) ;
}


void Server::onSessionStarted(impl::Session &session) {
    msg::SessionMetadata *session_metadata = new msg::SessionMetadata ;

    session_metadata->set_version(version()) ;

    msg::Message msg_body ;

    msg_body.set_allocated_session_metadata(session_metadata);

    ws_server_->send(session.connection_, msg_body.SerializeAsString());
}

void Server::dispatchUpdateMessage(const std::string &channel, const string &msg) {
    ws_server_->broadcast(channel, msg);
}



}
