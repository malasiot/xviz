#ifndef XVIZ_TABULAR_HPP
#define XVIZ_TABULAR_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include <sstream>

namespace xviz {

namespace msg {
    class Tabular ;
}
// Hierarchical data structure to store row/columns of arbitrary type

class TabularNode ;
class TabularData ;
class TabularColumn ;

class Tabular {
public:
    // list of columns
    Tabular(const std::vector<TabularColumn> &columns);
    ~Tabular() ;

    TabularNode *getRow(int row, const TabularNode *parent);
    TabularNode *addRow(const std::vector<TabularData> &data, TabularNode *parent = nullptr);

    TabularNode *root() const { return root_ ; }

    const std::vector<TabularColumn> &columns() const { return columns_ ; }

    static std::string write(const Tabular &t) ;
    static Tabular *read(const std::string &msg) ;
private:

    TabularNode *root_ ;
    std::vector<TabularColumn> columns_ ;
};

class TabularColumn {
public:
    TabularColumn(const std::string &id, const std::string &name= std::string()):
        id_(id), name_(name) {}

    const std::string &id() const { return id_ ; }
    const std::string &name() const { return name_ ; }

private:
    std::string id_, name_ ;
};

class TabularNode {
public:
    TabularNode(const std::vector<TabularData> &data, TabularNode *parent = nullptr);
    ~TabularNode() ;

    void addChildNode(TabularNode *c);

    TabularNode *child(int row) const {
        if (row < 0 || row >= children_.size()) return nullptr;
        return children_.at(row);
    }

    TabularNode *parent() const { return parent_ ; }

    const std::vector<TabularNode *> &children() const { return children_ ; }

    int row() const { return row_ ; }

    const std::vector<TabularData> &columns() const { return data_ ; }

private:
    int row_ = 0 ;
    std::vector<TabularData> data_ ; // column data
    TabularNode *parent_ ; // parent node in case of hierarchical data
    std::vector<TabularNode *> children_ ;
};

class TabularData {
public:

    using signed_integer_t = int64_t ;
    using unsigned_integer_t = uint64_t ;
    using float_t = double ;
    using string_t = std::string ;
    using boolean_t = bool ;

    enum class Type : uint8_t {
        Undefined, String, Boolean, UnsignedInteger, SignedInteger, Float, Double
    };

    // constructors

    TabularData(): type_(Type::Undefined) {}

    TabularData(bool v) noexcept : type_(Type::Boolean) { data_.b_ = v ; }

    TabularData(int32_t v) noexcept: TabularData((signed_integer_t)v) {}
    TabularData(uint32_t v) noexcept: TabularData((unsigned_integer_t)v) {}

    TabularData(int64_t v) noexcept: type_(Type::SignedInteger) { data_.i_ = v ; }
    TabularData(uint64_t v) noexcept: type_(Type::UnsignedInteger) { data_.u_ = v ; }

    TabularData(float v) noexcept: type_(Type::Float) { data_.d_ = v ; }
    TabularData(double v) noexcept: type_(Type::Double) { data_.d_ = v ; }

    TabularData(const char *value) {
        type_ = Type::String ;
        new (&data_.s_) string_t(value) ;
    }

    TabularData(const string_t& value) {
        type_ = Type::String ;
        new (&data_.s_) string_t(value) ;
    }

    TabularData(const char value) {
        type_ = Type::String ;
        new (&data_.s_) string_t(1, value) ;
    }

    TabularData(string_t&& value)  {
        type_ = Type::String ;
        new (&data_.s_) string_t(value) ;
    }

    ~TabularData() {
        destroy() ;
    }

    TabularData(const TabularData& other) {
        create(other) ;
    }

    TabularData &operator=(const TabularData &other) {
        if ( this != &other ) {
            destroy() ;
            create(other) ;
        }
        return *this ;
    }

    TabularData(TabularData&& other): type_(other.type_) {
        switch (type_)
        {
        case Type::String:
            new (&data_.s_) string_t(std::move(other.data_.s_)) ;
            break;
        case Type::Boolean:
            data_.b_ = other.data_.b_ ;
            break;
        case Type::UnsignedInteger:
            data_.u_ = other.data_.u_ ;
            break;
        case Type::SignedInteger:
            data_.i_ = other.data_.i_ ;
            break;
        case Type::Float:
        case Type::Double:
            data_.d_ = other.data_.d_ ;
            break;
        default:
            break;
        }

        other.type_ = Type::Undefined ;
    }

    // convert value to string
    std::string toString() const {
        switch (type_)
        {
        case Type::String:
            return data_.s_;
        case Type::Boolean: {
            std::ostringstream strm ;
            strm << data_.b_ ;
            return strm.str() ;
        }
        case Type::UnsignedInteger:
            return std::to_string(data_.u_) ;
        case Type::SignedInteger:
            return std::to_string(data_.i_) ;
        case Type::Float:
            return std::to_string((float)data_.d_) ;
        case Type::Double:
            return std::to_string((double)data_.d_) ;
        default:
            return std::string();
        }
    }

    float toFloat() const {
        switch (type_)
        {
        case Type::String:
            try {
            return std::stof(data_.s_);
        }
            catch ( ... ) {
            return 0.0f ;
        }

        case Type::Boolean:
            return (float)data_.b_ ;
        case Type::UnsignedInteger:
            return (float)data_.u_ ;
        case Type::SignedInteger:
            return (float)data_.i_ ;
        case Type::Float:
        case Type::Double:
            return (float)data_.d_ ;
        default:
            return 0.0f;
        }
    }

    double toDouble() const {
        switch (type_)
        {
        case Type::String:
            try {
            return std::stod(data_.s_);
        }
            catch ( ... ) {
            return 0.0 ;
        }

        case Type::Boolean:
            return (double)data_.b_ ;
        case Type::UnsignedInteger:
            return (double)data_.u_ ;
        case Type::SignedInteger:
            return (double)data_.i_ ;
        case Type::Float:
        case Type::Double:
            return (double)data_.d_ ;
        default:
            return 0.0;
        }
    }

    signed_integer_t toSignedInteger() const {
        switch (type_)
        {
        case Type::String:
            try {
            return std::stoll(data_.s_);
        }
            catch ( ... ) {
            return 0 ;
        }
        case Type::Boolean:
            return static_cast<signed_integer_t>(data_.b_) ;
        case Type::UnsignedInteger:
            return static_cast<signed_integer_t>(data_.u_) ;
        case Type::SignedInteger:
            return static_cast<signed_integer_t>(data_.i_) ;
        case Type::Float:
        case Type::Double:
            return static_cast<signed_integer_t>(data_.d_) ;
        default:
            return 0;
        }
    }

    unsigned_integer_t toUnsignedInteger() const {
        switch (type_)
        {
        case Type::String:
            try {
            return std::stoull(data_.s_);
        }
            catch ( ... ) {
            return 0 ;
        }
        case Type::Boolean:
            return static_cast<unsigned_integer_t>(data_.b_) ;
        case Type::UnsignedInteger:
            return static_cast<unsigned_integer_t>(data_.u_) ;
        case Type::SignedInteger:
            return static_cast<unsigned_integer_t>(data_.i_) ;
        case Type::Float:
        case Type::Double:
            return static_cast<unsigned_integer_t>(data_.d_) ;
        default:
            return 0;
        }
    }

    bool toBoolean() const {
        switch (type_)
        {
        case Type::String:
            return !(data_.s_.empty()) ;
        case Type::Boolean:
            return data_.b_ ;
        case Type::UnsignedInteger:
            return static_cast<bool>(data_.u_) ;
        case Type::SignedInteger:
            return static_cast<bool>(data_.u_) ;
        case Type::Float:
        case Type::Double:
            return static_cast<bool>(data_.d_ != 0.0) ;
        default:
            return false;
        }
    }

    Type type() const { return type_ ; }

    void destroy() {
        switch (type_) {
        case Type::String:
            data_.s_.~string_t() ;
            break ;
        }
    }

    void create(const TabularData &other) {
        type_ = other.type_ ;
        switch (type_)
        {
        case Type::String:
            new ( &data_.s_ ) string_t(other.data_.s_) ;
            break;
        case Type::Boolean:
            data_.b_ = other.data_.b_ ;
            break;
        case Type::UnsignedInteger:
            data_.u_ = other.data_.u_ ;
            break;
        case Type::SignedInteger:
            data_.i_ = other.data_.i_ ;
            break;
        case Type::Float:
        case Type::Double:
            data_.d_ = other.data_.d_ ;
            break;
        default:
            break;
        }

    }

private:

    union Data {
        string_t s_ ;
        boolean_t   b_ ;
        unsigned_integer_t u_ ;
        signed_integer_t   i_ ;
        float_t     d_ ;

        Data() {}
        ~Data() {}
    } ;

    Data data_ ;
    Type type_ ;
};
}

#endif
