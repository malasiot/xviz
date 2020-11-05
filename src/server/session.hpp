#ifndef XVIZ_SESSION_HPP
#define XVIZ_SESSION_HPP

#include <string>
#include <vector>

namespace xviz { namespace impl {

class Session {
public:
    uint64_t id_ ;
    std::string name_ ;
    std::vector<std::string> channels_ ;
};


}}









#endif
