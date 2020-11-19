#ifndef XVIZ_SCENE_MATERIAL_HPP
#define XVIZ_SCENE_MATERIAL_HPP

#include <string>
#include <Eigen/Geometry>

namespace xviz {

class Sampler2D {
public:
    std::string wrap_s_, wrap_t_ ;
};


class Texture2D {
public:

    Texture2D() = delete ;
    Texture2D(const std::string &url, const Sampler2D &sampler): image_url_(url), sampler_(sampler) {}

private:
    std::string image_url_ ;       // url should be file://<absolute path>
    Sampler2D sampler_ ;
};


class TextureOrColor {
public:
    TextureOrColor(const Eigen::Vector4f &clr): type_(Color) {
        new (&data_.c_) Eigen::Vector4f(clr) ;
    }

    TextureOrColor(const Texture2D &texture): type_(Texture) {
        new (&data_.t_) Texture2D(texture) ;
    }

    ~TextureOrColor() {
        destroy() ;
    }

    TextureOrColor(const TextureOrColor& other) {
        create(other) ;
    }

    TextureOrColor &operator=(const TextureOrColor &other) {
        if ( this != &other ) {
            destroy() ;
            create(other) ;
        }
        return *this ;
    }

private:

    using color_t = Eigen::Vector4f ;

    void destroy() {
        switch (type_) {
        case Type::Color:
            data_.c_.~color_t() ;
            break ;
        case Type::Texture:
            data_.t_.~Texture2D() ;
            break ;
        }
    }

    void create(const TextureOrColor &other) {
        type_ = other.type_ ;
        switch (type_)
        {
        case Type::Color:
            new ( &data_.c_ ) Eigen::Vector4f(other.data_.c_) ;
            break;
        case Type::Texture:
            new ( &data_.t_ ) Texture2D(other.data_.t_) ;
            break;
        default:
            break;
        }

    }

    enum Type { Texture, Color } ;

    union Data {
        Data() {} ;
        ~Data() {} ;
        Texture2D t_ ;
        Eigen::Vector4f c_ ;
    } ;

    Type type_ ;
    Data data_ ;

};

class Material {
public:
    virtual ~Material() = default ;
};


class PhongMaterial: public Material {
public:

    void setAmbient(const Eigen::Vector4f &a) { ambient_ = a ; }
    void setDiffuse(const Eigen::Vector4f &d) { diffuse_ = d ; }
    void setSpecular(const Eigen::Vector4f &s) { specular_ = s; }

    void setDiffuse(const Texture2D &d) { diffuse_ = d ; }
    void setShininess(float s) { shininess_ = s ; }


    ~PhongMaterial() = default ;
private:
    Eigen::Vector4f ambient_ = { 0, 0, 0, 1} ;
    TextureOrColor diffuse_ = Eigen::Vector4f{ 0.5, 0.5, 0.5, 1.0 };
    TextureOrColor specular_ = Eigen::Vector4f{ 0, 0, 0, 1 };
    float shininess_  = 1.0 ;

};


}
#endif
