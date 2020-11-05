#ifndef XVIZ_CHANNEL_HPP
#define XVIZ_CHANNEL_HPP

#include <string>
#include <memory>
#include <cassert>

namespace xviz {

class Channel {
public:
    enum Type {
        POINTS, IMAGE, TENSOR
    };

    Channel(const std::string &name, Type type, const std::string &desc = std::string()): name_(name), type_(type), description_(desc) {
        assert(!name.empty());
    }

    const std::string &name() const { return name_ ; }
    const Type type() const { return type_ ; }
    const std::string &description() const { return description_ ; }

private:
    std::string name_, description_ ;
    Type type_ ;
};






}
#endif
