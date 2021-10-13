#ifndef XVIZ_RENDERER_MATERIAL_HPP
#define XVIZ_RENDERER_MATERIAL_HPP

#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>

#include "shader.hpp"

#include <map>


enum MaterialProgramFlags { ENABLE_SKINNING = 1, ENABLE_SHADOWS = 2, HAS_DIFFUSE_TEXTURE = 4, HAS_SPECULAR_TEXTURE = 8 } ;

namespace xviz { namespace impl {

class MaterialProgram ;
using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;

template<class T, typename ...Args>
MaterialProgramPtr materialSingleton(std::map<int, MaterialProgramPtr> &instances, int flags, Args... args) {
    auto it = instances.find(flags) ;
    if ( it == instances.end() ) {
        std::shared_ptr<T> instance(new T(flags, args...)) ;
        instances.emplace(flags, instance) ;
        return instance ;
    } else {
        return it->second ;
    }
}


struct MaterialInstanceParams {
    GLuint num_shadow_lights_ =0 ;
    int flags = 0 ;
};

class MaterialProgram: public OpenGLShaderProgram {
public:

    virtual void applyParams(const MaterialPtr &mat) = 0 ;
    virtual void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) {}
    virtual void applyLight(const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat) {}
    virtual void applyBoneTransform(GLuint idx, const Eigen::Matrix4f &tf) ;

protected:

    void applyDefaultPerspective(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) ;
    void applyDefaultLight(const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat) ;

};

using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;


class PhongMaterialProgram: public MaterialProgram {
public:

    PhongMaterialProgram(int flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    void applyLight(const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat) override {
        applyDefaultLight(light, tf, lsmat) ;
    }

    static MaterialProgramPtr instance(int flags) {
        static std::map<int, MaterialProgramPtr> s_materials ;
        return materialSingleton<PhongMaterialProgram>(s_materials, flags) ;
    }

private:

    int flags_ = 0;
};


class ConstantMaterialProgram: public MaterialProgram {
public:

    ConstantMaterialProgram(int flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    void applyLight(const LightPtr &light, const Eigen::Affine3f &tf, const Eigen::Matrix4f &lsmat) override {
         applyDefaultLight(light, tf, lsmat) ;
    }

    static MaterialProgramPtr instance(int flags) {
        static std::map<int, MaterialProgramPtr> s_materials ;
        return materialSingleton<ConstantMaterialProgram>(s_materials, flags) ;
    }

private:

    int flags_ = 0;
};


class PerVertexColorMaterialProgram: public MaterialProgram {
public:

    PerVertexColorMaterialProgram(int flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    static MaterialProgramPtr instance(int flags) {
        static std::map<int, MaterialProgramPtr> s_materials ;
        return materialSingleton<PerVertexColorMaterialProgram>(s_materials, flags) ;
    }
};

class WireFrameMaterialProgram: public MaterialProgram {
public:

    WireFrameMaterialProgram(int flags) ;

    void applyParams(const MaterialPtr &mat) override ;

    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override {
        applyDefaultPerspective(cam, view, model) ;
    }

    static MaterialProgramPtr instance(int flags) {
        static std::map<int, MaterialProgramPtr> s_materials ;
        return materialSingleton<WireFrameMaterialProgram>(s_materials, flags) ;
    }
};

}}


#endif
