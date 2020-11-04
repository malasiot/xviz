#ifndef XVIZ_CHANNEL_HPP
#define XVIZ_CHANNEL_HPP

#include <string>

class Channel {
public:
    enum Type { PRIMITIVE, IMAGE } ;

    Type type_ ;
    std::string name_, description_ ;
};







#endif
