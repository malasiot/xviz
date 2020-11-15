#ifndef XVIZ_SERIALIZE_HPP
#define XVIZ_SERIALIZE_HPP

#include <string>
#include <memory>

namespace xviz {

template<class T>
class SerializerInterface {
public:
    virtual std::string write(const T *c) const = 0 ;
    virtual std::unique_ptr<T> read(const std::string &bytes) const = 0 ;
};

}
#endif
