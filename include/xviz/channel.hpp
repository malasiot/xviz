#ifndef XVIZ_CHANNEL_HPP
#define XVIZ_CHANNEL_HPP

#include <string>
#include <memory>

namespace xviz {

class Channel {
public:
    enum Type {
        POINTS, IMAGE
    };

    Channel(const std::string &name, Type type): name_(name), type_(type) {
    }

    const std::string name() const { return name_ ; }
    const Type type() const { return type_ ; }

private:
    std::string name_ ;
    Type type_ ;
};






}
#endif
