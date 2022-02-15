#ifndef XVIZ_RENDERER_MATERIAL_HPP
#define XVIZ_RENDERER_MATERIAL_HPP

#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>

#include "shader.hpp"
#include <xviz/scene/renderer.hpp>

#include <map>


enum MaterialProgramFlags { ENABLE_SKINNING = 1, ENABLE_SHADOWS = 2, HAS_DIFFUSE_TEXTURE = 4, HAS_SPECULAR_TEXTURE = 8 } ;

namespace xviz { namespace impl {

class MaterialProgram ;
using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;

struct MaterialInstanceParams {
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

template<class T, typename ...Args>
MaterialProgramPtr materialSingleton(std::map<std::string, MaterialProgramPtr> &instances, const MaterialInstanceParams &flags, Args... args) {
    const auto key = flags.key() ;
    auto it = instances.find(key) ;
    if ( it == instances.end() ) {
        std::shared_ptr<T> instance(new T(flags, args...)) ;
        instances.emplace(key, instance) ;
        return instance ;
    } else {
        return it->second ;
    }
}



class MaterialProgram: public OpenGLShaderProgram {
public:

    virtual void applyParams(const MaterialPtr &mat) = 0 ;
    virtual void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) {}
    virtual void applyLight(uint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) {}
    virtual void applyLights(const std::vector<LightData *> &lights) {}
    virtual void applyBoneTransform(GLuint idx, const Eigen::Matrix4f &tf) ;

protected:

    void applyDefaultPerspective(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) ;
    void applyDefaultLights(const std::vector<LightData *> &lights) ;

    void applyDirectionalLight(uint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) ;
    void applySpotLight(uint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) ;
    void applyPointLight(uint idx, const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat, GLuint tindex) ;
};

using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;


class PhongMaterialProgram: public MaterialProgram {
public:

    PhongMaterialProgram(const MaterialInstanceParams &flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    void applyLights(const std::vector<LightData *> &lights) override {
        applyDefaultLights(lights) ;
    }

    static MaterialProgramPtr instance(const MaterialInstanceParams &params) {
        static std::map<std::string, MaterialProgramPtr> s_materials ;
        return materialSingleton<PhongMaterialProgram>(s_materials, params) ;
    }

private:

    MaterialInstanceParams params_ ;
};


class ConstantMaterialProgram: public MaterialProgram {
public:

    ConstantMaterialProgram(const MaterialInstanceParams &) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    static MaterialProgramPtr instance(const MaterialInstanceParams &params) {
        static std::map<std::string, MaterialProgramPtr> s_materials ;
        return materialSingleton<ConstantMaterialProgram>(s_materials, params) ;
    }

private:

    MaterialInstanceParams params_;
};


class PerVertexColorMaterialProgram: public MaterialProgram {
public:

    PerVertexColorMaterialProgram(const MaterialInstanceParams &params) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    static MaterialProgramPtr instance(const MaterialInstanceParams &params) {
        static std::map<std::string, MaterialProgramPtr> s_materials ;
        return materialSingleton<PerVertexColorMaterialProgram>(s_materials, params) ;
    }
};

class WireFrameMaterialProgram: public MaterialProgram {
public:

    WireFrameMaterialProgram(const MaterialInstanceParams &flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    static MaterialProgramPtr instance(const MaterialInstanceParams &params) {
        static std::map<std::string, MaterialProgramPtr> s_materials ;
        return materialSingleton<WireFrameMaterialProgram>(s_materials, params) ;
    }
};

}}


#endif
