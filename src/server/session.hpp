#ifndef XVIZ_SESSION_HPP
#define XVIZ_SESSION_HPP

#include <string>
#include <vector>

#define ASIO_STANDALONE
#include <websocketpp/config/asio_no_tls.hpp>

namespace xviz { namespace impl {

class Session {
public:
    uint64_t id_ ;
    std::string name_ ;
    std::vector<std::string> channels_ ;
    websocketpp::connection_hdl connection_ ;
};


}}









#endif
