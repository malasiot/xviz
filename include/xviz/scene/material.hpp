#ifndef XVIZ_SCENE_MATERIAL_HPP
#define XVIZ_SCENE_MATERIAL_HPP

#include <string>
#include <memory>
#include <Eigen/Geometry>

#include <xviz/common/image.hpp>

namespace xviz {


class Sampler2D {
public:
    enum TextureMapMode { WRAP, CLAMP, DECAL } ;

    Sampler2D() = default ;
    Sampler2D(TextureMapMode wrap_u, TextureMapMode wrap_v):
        wrap_u_(wrap_u), wrap_v_(wrap_v) {}

    TextureMapMode mapModeU() const { return wrap_u_; }
    TextureMapMode mapModeV() const { return wrap_v_; }

private:
    TextureMapMode wrap_u_ = WRAP, wrap_v_ = WRAP ;
};

class Texture2D {
public:
    Texture2D() = delete ;
    Texture2D(const Image &image, const Sampler2D &sampler): image_(image), sampler_(sampler) {}

    const Image &image() const { return image_ ; }
    const Sampler2D &sampler() const { return sampler_ ; }

private:
    Image image_ ;
    Sampler2D sampler_ ;
};



class Material {
public:
    enum class Side { Front, Back, Both } ;

    virtual ~Material() = default ;

    void setSide(Side s) { side_ = s ; }
    Side side() const { return side_ ; }

private:
    Side side_ = Side::Front ;
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

class ConstantMaterial: public Material {
public:
    ConstantMaterial(const Eigen::Vector4f &clr): clr_(clr) {}

    const Eigen::Vector4f &color() const { return clr_ ; }

private:
    Eigen::Vector4f clr_ ;
};

class PerVertexColorMaterial: public Material {
public:
    PerVertexColorMaterial(float opacity = 1.0): opacity_(opacity) {}

    float opacity() const { return opacity_ ; }

private:
    float opacity_ = 1.0;
};

class WireFrameMaterial: public Material {
public:
    WireFrameMaterial(const Eigen::Vector4f &clr, const Eigen::Vector4f &fill, float lw = 1.0f): line_clr_(clr), fill_clr_(fill), line_width_(lw) {}

    float lineWidth() const { return line_width_ ; }
    const Eigen::Vector4f &lineColor() const { return line_clr_ ; }
    const Eigen::Vector4f &fillColor() const { return fill_clr_ ; }

private:
    float line_width_ ;
    Eigen::Vector4f line_clr_, fill_clr_ ;
};

}
#endif
