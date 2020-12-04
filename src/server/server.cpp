#include <xviz/server.hpp>
#include "ws_server.hpp"
#include "session.hpp"
#include "session.pb.h"
#include <xviz/image.hpp>
#include <xviz/tabular.hpp>

#include <chrono>

using namespace std ;

namespace xviz {

Server::Server(const string &doc_root): ws_server_(new impl::WebSocketServer(this, doc_root)) {

}

Server::~Server() {}

void Server::run(int port) {
    ws_server_->run(port) ;
}

static double clock() {
    using namespace chrono ;

    high_resolution_clock::time_point epoch;
    auto now = high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed(now - epoch);
    return elapsed.count() ;
}


void Server::sendUpdateMessage(const std::string &channel, const string &data, ChannelUpdateAction action, const std::string &object_id) {
    msg::StateUpdate *update = new msg::StateUpdate() ;
    update->set_ts(clock());
    update->set_channel_id(channel) ;
    update->set_data(data) ;

    if ( !object_id.empty() )
        update->set_object_id(object_id) ;

    switch ( action ) {
    case ChannelAppendAction:
        update->set_action(msg::StateUpdate_Action_ADD) ;
        break ;
    case ChannelReplaceAction:
        update->set_action(msg::StateUpdate_Action_REPLACE) ;
        break ;
    case ChannelDeleteAction:
        update->set_action(msg::StateUpdate_Action_DELETE) ;
        break ;
    }

    msg::Message msg ;
    msg.set_allocated_state_update(update);
    dispatchUpdateMessage(channel, msg.SerializeAsString());
}


void Server::push(const string &channel, const Message &msg, ChannelUpdateAction a, const string &object_id) {
    sendUpdateMessage(channel, msg.encode(), a, object_id) ;
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
