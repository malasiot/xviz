#include "material.hpp"

PhongMaterialProgram::PhongMaterialProgram(int flags)
{
#if 0
    std::string preproc ;
    preproc.append("#define HAS_NORMALS\n") ;
    if ( flags_ & USE_SKINNING )  preproc.append("#define USE_SKINNING\n") ;
    if ( flags_ & HAS_SHADOWS )  preproc.append("#define HAS_SHADOWS\n") ;

    vs->addSourceString(version_header) ;
    vs->addSourceString(preproc) ;
    vs->addSourceString(vertex_shader_code, "vertex_shader_code") ;

    gl::Shader::Ptr fs(new gl::Shader(gl::Shader::Fragment)) ;

    fs->addSourceString(version_header) ;
    fs->addSourceString(preproc) ;
    fs->addSourceString(phong_fragment_shader_common) ;
    fs->addSourceString(phong_fragment_shader_material, "phong_fragment_shader_material")  ;

    prog_.reset(new gl::ShaderProgram) ;
    prog_->addShader(vs) ;
    prog_->addShader(fs) ;

    prog_->link() ;
#endif
}

void PhongMaterialProgram::applyParams(const xviz::MaterialPtr &mat)
{

}

void PhongMaterialProgram::applyTransform(const Eigen::Matrix4f &cam, const Eigen::Matrix4f &view, const Eigen::Matrix4f &model)
{

}

void PhongMaterialProgram::applyLight(uint idx, const xviz::LightPtr &light, const Eigen::Affine3f &tf)
{

}

void MaterialProgram::use() {
    prog_.bind() ;
}
