#ifndef XVIZ_SCENE_MATERIAL_HPP
#define XVIZ_SCENE_MATERIAL_HPP

#include <string>
#include <memory>
#include <Eigen/Geometry>

#include <xviz/common/image.hpp>

namespace xviz {

namespace impl {
class MaterialProgram ;
struct MaterialProgramParams ;
using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;
}

class Sampler2D {
public:
    enum TextureWrapMode { WRAP_REPEAT, WRAP_CLAMP, WRAP_MIRROR_REPEAT } ;
    enum MagnificationMode { MAG_NEAREST, MAG_LINEAR } ;
    enum MinificationMode { MIN_NEAREST,  MIN_LINEAR,
                            MIN_NEAREST_MIPMAP_NEAREST,
                            MIN_LINEAR_MIPMAP_NEAREST,
                            MIN_NEAREST_MIPMAP_LINEAR,
                            MIN_LINEAR_MIPMAP_LINEAR } ;


    Sampler2D() = default ;
    Sampler2D(TextureWrapMode wrap_u, TextureWrapMode wrap_v):
        wrap_u_(wrap_u), wrap_v_(wrap_v) {}

    void setMagnification(MagnificationMode m) {
        mag_ = m ;
    }

    void setMinification(MinificationMode m) {
        min_ = m ;
    }

    MagnificationMode magnification() const { return mag_ ; }
    MinificationMode minification() const { return min_ ; }

    TextureWrapMode wrapModeU() const { return wrap_u_; }
    TextureWrapMode wrapModeV() const { return wrap_v_; }

    bool generateMipMaps() const { return mipmaps_ ; }
    void setGenerateMipMaps(bool v) { mipmaps_ = v ; }

private:
    TextureWrapMode wrap_u_ = WRAP_REPEAT, wrap_v_ = WRAP_REPEAT ;
    MagnificationMode mag_ = MAG_LINEAR ;
    MinificationMode min_ = MIN_LINEAR_MIPMAP_LINEAR ;
    bool mipmaps_ = true ;
};

class Texture2D {
public:
    Texture2D() = delete ;
    Texture2D(const std::shared_ptr<Image> &image, const Sampler2D &sampler): image_(image), sampler_(sampler) {}

    const ImagePtr &image() const { return image_ ; }
    const Sampler2D &sampler() const { return sampler_ ; }

    void setTransform(const Eigen::Affine2f &tr) { transform_ = tr ; }

    Eigen::Affine2f transform() const { return transform_ ; }

private:

    std::shared_ptr<Image> image_ ;
    Sampler2D sampler_ ;
    Eigen::Affine2f transform_ = Eigen::Affine2f::Identity() ;
};



class Material {
public:
    enum class Side { Front, Back, Both } ;

    virtual ~Material() = default ;

    void setSide(Side s) { side_ = s ; }
    Side side() const { return side_ ; }

    void enableDepthTest(bool e) { enable_depth_test_ = e ; }
    bool hasDepthTest() const { return enable_depth_test_ ; }

    virtual bool hasTexture() const { return false ; }

    virtual impl::MaterialProgramPtr instantiate(const impl::MaterialProgramParams &) const = 0 ;

private:
    friend class Renderer ;


    Side side_ = Side::Front ;
    bool enable_depth_test_ = true ;
};


class PhongMaterial: public Material {
public:

    PhongMaterial() = default ;
    PhongMaterial(const Eigen::Vector3f &diffuse, float opacity = 1.0): diffuse_clr_(diffuse), opacity_(opacity) {}


    PhongMaterial(const Eigen::Vector4f &diffuse): diffuse_clr_(diffuse.block<3, 1>(0, 0)), opacity_(diffuse[3]) {}

    void setAmbientColor(const Eigen::Vector3f &a) { ambient_ = a ; }
    void setDiffuseColor(const Eigen::Vector3f &d) { diffuse_clr_ = d ; }
    void setSpecularColor(const Eigen::Vector3f &s) { specular_clr_ = s; }

    void setDiffuseTexture(Texture2D *d) { diffuse_map_.reset(d) ; }
    void setSpecularTexture(Texture2D *d) { specular_map_.reset(d) ; }
    void setShininess(float s) { shininess_ = s ; }
    void setOpacity(float s) { opacity_ = s ; }

    const Eigen::Vector3f &diffuseColor() const { return diffuse_clr_ ; }
    const Eigen::Vector3f &specularColor() const { return specular_clr_ ; }
    const Eigen::Vector3f &ambientColor() const { return ambient_ ; }
    float shininess() const { return shininess_ ; }
    float opacity() const { return opacity_ ; }

    const Texture2D *diffuseTexture() const { return diffuse_map_.get() ; }
    const Texture2D *specularTexture() const { return specular_map_.get() ; }

    bool hasTexture() const override { return diffuse_map_ != nullptr ;  }

    ~PhongMaterial() = default ;
private:
    impl::MaterialProgramPtr instantiate(const impl::MaterialProgramParams &) const override ;

    Eigen::Vector3f ambient_ = { 0, 0, 0 } ;
    Eigen::Vector3f diffuse_clr_{ 0.5, 0.5, 0.5 };
    Eigen::Vector3f specular_clr_{ 0, 0, 0 };
    std::unique_ptr<Texture2D> diffuse_map_, specular_map_ ;
    float shininess_  = 1.0 ;
    float opacity_ = 1.0 ;
};

class ConstantMaterial: public Material {
public:
    ConstantMaterial(const Eigen::Vector4f &clr): clr_(clr) {}

    void setColor(const Eigen::Vector4f &clr) {
        clr_ = clr ;
    }

    void setTexture(Texture2D *tex) { texture_.reset(tex) ; }

    const Eigen::Vector4f &color() const { return clr_ ; }

    const Texture2D *texture() const { return texture_.get() ; }

    bool hasTexture() const override { return texture_ != nullptr ;  }

private:

    impl::MaterialProgramPtr instantiate(const impl::MaterialProgramParams &) const override ;

    Eigen::Vector4f clr_ ;
    std::unique_ptr<Texture2D> texture_ ;
};

class PerVertexColorMaterial: public Material {
public:
    PerVertexColorMaterial(float opacity = 1.0): opacity_(opacity) {}

    float opacity() const { return opacity_ ; }

private:
    impl::MaterialProgramPtr instantiate(const impl::MaterialProgramParams &) const override ;

    float opacity_ = 1.0;
};

class WireFrameMaterial: public Material {
public:
    WireFrameMaterial(const Eigen::Vector4f &clr, const Eigen::Vector4f &fill, float lw = 1.0f): line_clr_(clr), fill_clr_(fill), line_width_(lw) {}

    float lineWidth() const { return line_width_ ; }
    const Eigen::Vector4f &lineColor() const { return line_clr_ ; }
    const Eigen::Vector4f &fillColor() const { return fill_clr_ ; }

private:

    impl::MaterialProgramPtr instantiate(const impl::MaterialProgramParams &) const override ;

    float line_width_ ;
    Eigen::Vector4f line_clr_, fill_clr_ ;
};

}
#endif
