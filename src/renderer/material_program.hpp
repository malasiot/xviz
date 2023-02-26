#ifndef XVIZ_RENDERER_MATERIAL_HPP
#define XVIZ_RENDERER_MATERIAL_HPP

#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>

#include "common/shader.hpp"
#include <xviz/scene/renderer.hpp>
#include <xviz/scene/material.hpp>

#include <map>


enum MaterialProgramFlags { ENABLE_SKINNING = 1, ENABLE_SHADOWS = 2, HAS_DIFFUSE_TEXTURE = 4, HAS_SPECULAR_TEXTURE = 8 } ;

namespace xviz { namespace impl {

class TextureData ;
struct LightData ;
class MaterialProgram ;
using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;

// The  program corresponding to each material may have variations (e.g. depending on shadows, skinning)
// Therefore each program is instantiated with a set of params. From the parameters a unique key is extracted
// and then its unique program is associated with these key

class MaterialProgramManager {
public:

    MaterialProgramManager() = default;

    template<class T, class P, typename ...Args>
    MaterialProgramPtr instance(const P &params, Args... args) {
        const auto key = params.key() ;
        auto it = instances_.find(key) ;
        if ( it == instances_.end() ) {
            std::shared_ptr<T> instance(new T(params, args...)) ;
            instances_.emplace(key, instance) ;
            return instance ;
        } else {
            return it->second ;
        }
    }

private:
    std::map<std::string, MaterialProgramPtr> instances_ ;
};

using TextureLoader = std::function<impl::TextureData *(const Texture2D *)> ;

class MaterialProgram: public OpenGLShaderProgram {
public:

    virtual void applyParams(const MaterialPtr &mat) = 0 ;
    virtual void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) {}
    virtual void applyLights(const std::vector<LightData *> &lights) {}
    virtual void applyBoneTransform(GLuint idx, const Eigen::Matrix4f &tf) ;
    virtual void bindTextures(const MaterialPtr &, TextureLoader) {}

protected:

    void applyDefaultPerspective(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) ;
    void applyDefaultLights(const std::vector<LightData *> &lights) ;

    void applyDirectionalLight(GLuint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) ;
    void applySpotLight(GLuint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) ;
    void applyPointLight(GLuint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) ;
    void bindTexture(const Texture2D *texture, TextureLoader loader, int slot);

};

using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;


class PhongMaterialProgram: public MaterialProgram {
public:

    struct Params {
        GLuint num_dir_lights_ = 0 ;
        GLuint num_dir_lights_shadow_ = 0 ;
        GLuint num_point_lights_ = 0 ;
        GLuint num_point_lights_shadow_ = 0 ;
        GLuint num_spot_lights_ = 0 ;
        GLuint num_spot_lights_shadow_ = 0 ;

        bool enable_shadows_ = false ;
        bool enable_skinning_ = false ;
        bool has_diffuse_map_ = false ;

        std::string key() const ;
    };

    PhongMaterialProgram(const Params &flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    void applyLights(const std::vector<LightData *> &lights) override {
        applyDefaultLights(lights) ;
    }

    void bindTextures(const MaterialPtr &, TextureLoader) override ;


private:

    Params params_ ;
};


class ConstantMaterialProgram: public MaterialProgram {
public:
    struct Params {
        bool enable_skinning_ = false ;
        bool has_texture_map_ = false ;

        std::string key() const ;
    };

    ConstantMaterialProgram(const Params &p) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    void bindTextures(const MaterialPtr &mat, TextureLoader loader) override ;
private:

    Params params_;
};


class PerVertexColorMaterialProgram: public MaterialProgram {
public:

    struct Params {
        bool enable_skinning_ = false ;

        std::string key() const ;
    };

    PerVertexColorMaterialProgram(const Params &params) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

};

class WireFrameMaterialProgram: public MaterialProgram {
public:

    struct Params {
        bool enable_skinning_ = false ;

        std::string key() const ;
    };

    WireFrameMaterialProgram(const Params &flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }


};

}}


#endif
