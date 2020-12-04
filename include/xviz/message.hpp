#ifndef XVIZ_MESSAGE_HPP
#define XVIZ_MESSAGE_HPP

#include <string>

namespace xviz {

class MessageCodec ;

// all data that can be transmited in an update message should inherit Message

class Message {
public:
    virtual ~Message() {}
    virtual std::string encode() const = 0 ;
protected:
    Message() = default ;
};


}

#endif
