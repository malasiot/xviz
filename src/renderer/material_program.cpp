#include "material_program.hpp"

#include <clsim/scene/material.hpp>
#include <clsim/scene/light.hpp>

#include "util.hpp"

using namespace std ;
using namespace Eigen ;


#include "shaders/common.vs.hpp"
#include "shaders/phong.fs.hpp"
#include "shaders/shadows.fs.hpp"

namespace clsim { namespace impl {

static const char *version_header = "#version 330\n" ;

PhongMaterialProgram::PhongMaterialProgram(int flags): flags_(flags)
{
    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;

    std::string vs_preproc ;
    vs_preproc.append("#define HAS_NORMALS\n") ;
    if ( flags & HAS_DIFFUSE_TEXTURE ) vs_preproc.append("#define HAS_UVs\n") ;
    if ( flags & ENABLE_SKINNING ) vs_preproc.append("#define USE_SKINNING\n");
    if ( flags & ENABLE_SHADOWS ) vs_preproc.append("#define HAS_SHADOWS\n") ;

    vs->addSourceString(version_header) ;
    vs->addSourceString(vs_preproc) ;
    vs->addSourceString(vertex_shader_code, "vertex_shader_code") ;

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER)) ;

    std::string fs_preproc ;

    if ( flags & HAS_DIFFUSE_TEXTURE )
        fs_preproc.append("#define HAS_DIFFUSE_MAP") ;

    if ( flags & ENABLE_SHADOWS )
        fs_preproc.append("#define HAS_SHADOWS\n") ;

    fs->addSourceString(version_header) ;
    fs->addSourceString(fs_preproc) ;
    fs->addSourceString(phong_fragment_shader_vars) ;

    if ( flags & ENABLE_SHADOWS )
        fs->addSourceString(shadows_fragment_shader) ;

    fs->addSourceString(phong_fragment_shader_common) ;
    fs->addSourceString(phong_fragment_shader);

    addShader(vs) ;
    addShader(fs) ;

    link() ;
}

void PhongMaterialProgram::applyParams(const MaterialPtr &mat) {
    const PhongMaterial *material = dynamic_cast<const PhongMaterial *>(mat.get());
    if ( material == nullptr ) return ;

    setUniform("g_material.ambient", material->ambientColor()) ;
    setUniform("g_material.specular", material->specularColor()) ;
    setUniform("g_material.shininess", material->shininess());
    setUniform("g_material.diffuse", material->diffuseColor());

    if ( flags_ & HAS_DIFFUSE_TEXTURE ) {
        setUniform("diffuseMap", 0) ;
    }
}


void MaterialProgram::applyBoneTransform(uint idx, const Matrix4f &tf)
{
    ostringstream name ;
    name << "g_bones[" << idx << "]" ;
    setUniform(name.str().c_str(), tf) ;
}

void MaterialProgram::applyDefaultPerspective(const Matrix4f &cam, const Matrix4f &view, const Matrix4f &model) {
    Matrix4f mvp =  cam * view * model;
    Matrix4f mv =   view * model;

    Matrix3f wpi = mv.block<3, 3>(0, 0).transpose().eval() ;
    Matrix3f wp(wpi.inverse().eval()) ;

    setUniform("mvp", mvp) ;
    setUniform("mv", mv) ;
    setUniform("mvn", wp) ;
    setUniform("model", model) ;
}


void MaterialProgram::applyDefaultLight(const LightPtr &light, const Affine3f &tf, const Matrix4f &lsmat)
{
    string vname("g_light_source") ;

    if ( const auto &alight = std::dynamic_pointer_cast<AmbientLight>(light) ) {

        setUniform(vname + ".light_type", 0) ;
        setUniform(vname + ".color", alight->color_) ;
    }
    else if ( const auto &dlight = std::dynamic_pointer_cast<DirectionalLight>(light) ) {
        setUniform(vname + ".light_type", 1) ;
        setUniform(vname + ".color", dlight->diffuse_color_) ;
        setUniform(vname + ".direction", tf * (dlight->position_ - dlight->target_).normalized()) ;
    }
    else if ( const auto &slight = std::dynamic_pointer_cast<SpotLight>(light) ) {

        setUniform(vname + ".light_type", 2) ;
        setUniform(vname + ".color", slight->diffuse_color_) ;
        setUniform(vname + ".direction", tf * slight->direction_) ;
        setUniform(vname + ".position", tf * slight->position_) ;
        setUniform(vname + ".constant_attenuation", slight->constant_attenuation_) ;
        setUniform(vname + ".linear_attenuation", slight->linear_attenuation_) ;
        setUniform(vname + ".quadratic_attenuation", slight->quadratic_attenuation_) ;
        setUniform(vname + ".spot_exponent", slight->falloff_exponent_) ;
        setUniform(vname + ".spot_cos_cutoff", (float)cos(M_PI*slight->falloff_angle_/180.0)) ;
    }
    else if ( const auto &plight = std::dynamic_pointer_cast<PointLight>(light)) {
        setUniform(vname + ".light_type", 3) ;
        setUniform(vname + ".color", plight->diffuse_color_) ;
        setUniform(vname + ".position", tf * plight->position_) ;
        setUniform(vname + ".constant_attenuation", plight->constant_attenuation_) ;
        setUniform(vname + ".linear_attenuation", plight->linear_attenuation_) ;
        setUniform(vname + ".quadratic_attenuation", plight->quadratic_attenuation_) ;
    }

    setUniform("light_casts_shadows", light->casts_shadows_) ;

    if ( light->casts_shadows_ ) {
        setUniform("lsmat", Matrix4f(lsmat)) ;
        setUniform("shadowMap", 4) ;
        setUniform("shadowBias", light->shadow_bias_) ;
    }
}


#include "shaders/constant.fs.hpp"

ConstantMaterialProgram::ConstantMaterialProgram(int flags): flags_(flags) {
    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;

    std::string preproc ;
    if ( flags & ENABLE_SKINNING ) preproc.append("#define USE_SKINNING\n");
    if ( flags & ENABLE_SHADOWS ) preproc.append("#define HAS_SHADOWS\n") ;

    vs->addSourceString(version_header) ;
    vs->addSourceString(preproc) ;
    vs->addSourceString(vertex_shader_code, "vertex_shader_code") ;

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER)) ;

    fs->addSourceString(version_header) ;
    fs->addSourceString(preproc) ;
    fs->addSourceString(constant_fragment_shader_vars) ;
    if ( flags & ENABLE_SHADOWS )
        fs->addSourceString(shadows_fragment_shader) ;
    fs->addSourceString(constant_fragment_shader) ;

    addShader(vs) ;
    addShader(fs) ;

    link() ;
}

void ConstantMaterialProgram::applyParams(const MaterialPtr &mat) {
    const ConstantMaterial *material = dynamic_cast<const ConstantMaterial *>(mat.get());
    assert( material ) ;

    setUniform("color", material->color()) ;
}

#include "shaders/per_vertex.fs.hpp"

PerVertexColorMaterialProgram::PerVertexColorMaterialProgram(int flags) {

    OpenGLShaderPtr vs(new OpenGLShader(VERTEX_SHADER)) ;

    std::string preproc ;
    preproc.append("#define HAS_COLORS");
    if ( flags & ENABLE_SKINNING ) preproc.append("#define USE_SKINNING");

    vs->addSourceString(version_header) ;
    vs->addSourceString(preproc) ;
    vs->addSourceString(vertex_shader_code, "vertex_shader_code") ;

    OpenGLShaderPtr fs(new OpenGLShader(FRAGMENT_SHADER)) ;

    fs->addSourceString(version_header) ;
    fs->addSourceString(preproc) ;
    fs->addSourceString(per_vertex_color_fragment_shader) ;

    addShader(vs) ;
    addShader(fs) ;

    link() ;
}

void PerVertexColorMaterialProgram::applyParams(const MaterialPtr &mat) {
    const PerVertexColorMaterial *material = dynamic_cast<const PerVertexColorMaterial *>(mat.get());
    assert( material ) ;

     setUniform("opacity", material->opacity()) ;
}

}}
