#ifndef XVIZ_SCENE_MATERIAL_HPP
#define XVIZ_SCENE_MATERIAL_HPP

#include <string>
#include <memory>
#include <Eigen/Geometry>

#include <xviz/image.hpp>

namespace xviz {

namespace msg {
    class Texture2D ;
    class Sampler2D ;
}

class Sampler2D {
public:
    enum TextureMapMode { WRAP, CLAMP, DECAL } ;

    Sampler2D(TextureMapMode wrap_u, TextureMapMode wrap_v):
        wrap_u_(wrap_u), wrap_v_(wrap_v) {}

    TextureMapMode mapModeU() const { return wrap_u_; }
    TextureMapMode mapModeV() const { return wrap_v_; }

    static msg::Sampler2D *write(const Sampler2D &s) ;
    static Sampler2D *read(const msg::Sampler2D &msg) ;

private:
    TextureMapMode wrap_u_ = WRAP, wrap_v_ = WRAP ;
};

class Texture2D {
public:
    Texture2D() = delete ;
    Texture2D(const xviz::Image &image, const Sampler2D &sampler): image_(image), sampler_(sampler) {}

    const xviz::Image &image() const { return image_ ; }
    const xviz::Sampler2D &sampler() const { return sampler_ ; }

    static msg::Texture2D *write(const Texture2D &t) ;
    static Texture2D *read(const msg::Texture2D &msg) ;

private:
    xviz::Image image_ ;
    Sampler2D sampler_ ;
};



class Material {
public:
    virtual ~Material() = default ;
};


class PhongMaterial: public Material {
public:

    PhongMaterial() = default ;
    PhongMaterial(const Eigen::Vector4f &diffuse): diffuse_clr_(diffuse) {}

    void setAmbientColor(const Eigen::Vector4f &a) { ambient_ = a ; }
    void setDiffuseColor(const Eigen::Vector4f &d) { diffuse_clr_ = d ; }
    void setSpecularColor(const Eigen::Vector4f &s) { specular_clr_ = s; }

    void setDiffuseTexture(Texture2D *d) { diffuse_map_.reset(d) ; }
    void setSpecularTexture(Texture2D *d) { specular_map_.reset(d) ; }
    void setShininess(float s) { shininess_ = s ; }

    const Eigen::Vector4f &diffuseColor() const { return diffuse_clr_ ; }
    const Eigen::Vector4f &specularColor() const { return specular_clr_ ; }
    const Eigen::Vector4f &ambientColor() const { return ambient_ ; }
    float shininess() const { return shininess_ ; }

    const Texture2D *diffuseTexture() const { return diffuse_map_.get() ; }
    const Texture2D *specularTexture() const { return specular_map_.get() ; }

    ~PhongMaterial() = default ;
private:
    Eigen::Vector4f ambient_ = { 0, 0, 0, 1} ;
    Eigen::Vector4f diffuse_clr_{ 0.5, 0.5, 0.5, 1.0 };
    Eigen::Vector4f specular_clr_{ 0, 0, 0, 1 };
    std::unique_ptr<Texture2D> diffuse_map_, specular_map_ ;
    float shininess_  = 1.0 ;
};


}
#endif
