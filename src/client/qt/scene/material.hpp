#ifndef XVIZ_QT_MATERIAL_HPP
#define XVIZ_QT_MATERIAL_HPP

#include <Eigen/Geometry>
#include <xviz/scene/scene_fwd.hpp>

#include <QOpenGLShaderProgram>
#include <map>

class MaterialProgram ;
using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;


template<class T>
MaterialProgramPtr materialSingleton(std::map<int, MaterialProgramPtr> &instances, int flags) {
    auto it = instances.find(flags) ;
    if ( it == instances.end() ) {
        std::shared_ptr<T> instance(new T(flags)) ;
        instances.emplace(flags, instance) ;
        return instance ;
    } else {
        return it->second ;
    }
}

class MaterialProgram {
public:

    virtual void applyParams(const xviz::MaterialPtr &mat) = 0 ;
    virtual void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) {}
    virtual void applyLight(uint idx, const xviz::LightPtr &light, const Eigen::Affine3f &tf) {}
    virtual void applyBoneTransform(uint idx, const Eigen::Matrix4f &tf) {}

    void use() ;
protected:

    void applyDefaultPerspective(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) ;
    void applyDefaultLight(uint idx, const xviz::LightPtr &light, const Eigen::Affine3f &tf) ;
    void applyDefaultShadow(const Eigen::Matrix4f &ls, float bias);

    int flags_ ;

protected:


    QOpenGLShaderProgram prog_ ;
};

using MaterialProgramPtr = std::shared_ptr<MaterialProgram> ;

class PhongMaterialProgram: public MaterialProgram {
public:
    enum Flags { HAS_DIFFUSE_TEXTURE = 1 } ;

    PhongMaterialProgram(int flags) ;

    void applyParams(const xviz::MaterialPtr &mat) override ;
    void applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model) override ;
    void applyLight(uint idx, const xviz::LightPtr &light, const Eigen::Affine3f &tf) override ;

    static MaterialProgramPtr instance(int flags) {
        static std::map<int, MaterialProgramPtr> s_materials ;
        return materialSingleton<PhongMaterialProgram>(s_materials, flags) ;
    }
};



#endif
