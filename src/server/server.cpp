#include <xviz/server.hpp>
#include "ws_server.hpp"
#include "session.hpp"
#include "session.pb.h"

using namespace std ;

namespace xviz {

Server::Server(const string &doc_root): ws_server_(new impl::WebSocketServer(this, doc_root)) {

}

Server::~Server() {}

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

void Server::sendImageUri(Channel *channel, const string &uri)
{
    assert(channel->type() == Channel::IMAGE ) ;

    msg::StateUpdate *update = new msg::StateUpdate() ;
    update->set_ts(0.0);
    update->set_channel_id(channel->name()) ;

    msg::ImageData *img_data = new msg::ImageData() ;
    img_data->set_url(uri);

    update->set_allocated_image_data(img_data);

    msg::Message msg ;

    msg.set_allocated_state_update(update);

    dispatchUpdateMessage(channel, msg.SerializeAsString());

}


void Server::onSessionStarted(impl::Session &session) {
    msg::SessionConfig *session_cfg = new msg::SessionConfig() ;

    for( const auto &channel: session.channels_ ) {
        Channel *ch = findChannel(channel) ;
        if ( ch != nullptr ) {
            msg::ChannelInfo *info = session_cfg->add_channel_info() ;
            info->set_name(channel);
            info->set_description(ch->description());
            info->set_type((int)ch->type()) ;
        }
    }

    msg::Message msg_body ;

    msg_body.set_allocated_session_config(session_cfg);

    ws_server_->send(session.connection_, msg_body.SerializeAsString());
}

void Server::dispatchUpdateMessage(Channel *c, const string &msg)
{
    ws_server_->broadcast(c->name(), msg);
}



}
