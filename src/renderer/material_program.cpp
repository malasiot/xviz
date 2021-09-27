#include "material_program.hpp"

#include <xviz/scene/material.hpp>
#include <xviz/scene/light.hpp>

#include "util.hpp"

using namespace std ;
using namespace Eigen ;

namespace xviz { namespace impl {

PhongMaterialProgram::PhongMaterialProgram(int flags): flags_(flags)
{
    OpenGLShaderPreproc vs_preproc ;

    vs_preproc.appendDefinition("HAS_NORMALS") ;

    if ( flags & HAS_DIFFUSE_TEXTURE ) vs_preproc.appendDefinition("HAS_UVs") ;
    if ( flags & ENABLE_SKINNING ) vs_preproc.appendDefinition("USE_SKINNING");
    if ( flags & ENABLE_SHADOWS ) vs_preproc.appendDefinition("HAS_SHADOWS") ;

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", vs_preproc) ;

    OpenGLShaderPreproc fs_preproc ;

    if ( flags & HAS_DIFFUSE_TEXTURE )
        fs_preproc.appendDefinition("HAS_DIFFUSE_MAP") ;

    if ( flags & ENABLE_SHADOWS )
        fs_preproc.appendDefinition("HAS_SHADOWS") ;

    addShaderFromFile(FRAGMENT_SHADER, "@phong_fragment_shader", fs_preproc) ;

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

ConstantMaterialProgram::ConstantMaterialProgram(int flags): flags_(flags) {
    OpenGLShaderPreproc preproc ;

    if ( flags & ENABLE_SKINNING ) preproc.appendDefinition("USE_SKINNING");
    if ( flags & ENABLE_SHADOWS ) preproc.appendDefinition("HAS_SHADOWS") ;

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@constant_fragment_shader", preproc) ;

    link() ;
}

void ConstantMaterialProgram::applyParams(const MaterialPtr &mat) {
    const ConstantMaterial *material = dynamic_cast<const ConstantMaterial *>(mat.get());
    assert( material ) ;

    setUniform("color", material->color()) ;
}

PerVertexColorMaterialProgram::PerVertexColorMaterialProgram(int flags) {

    OpenGLShaderPreproc preproc ;
    preproc.appendDefinition("HAS_COLORS");
    if ( flags & ENABLE_SKINNING ) preproc.appendDefinition("USE_SKINNING");

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@per_vertex_color_fragment_shader", preproc) ;

    link() ;
}

void PerVertexColorMaterialProgram::applyParams(const MaterialPtr &mat) {
    const PerVertexColorMaterial *material = dynamic_cast<const PerVertexColorMaterial *>(mat.get());
    assert( material ) ;

     setUniform("opacity", material->opacity()) ;
}


WireFrameMaterialProgram::WireFrameMaterialProgram(int flags) {

    OpenGLShaderPreproc preproc ;

    if ( flags & ENABLE_SKINNING ) preproc.appendDefinition("USE_SKINNING");

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(GEOMETRY_SHADER, "@wireframe_geometry_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@wireframe_fragment_shader", preproc) ;

    link() ;
}

void WireFrameMaterialProgram::applyParams(const MaterialPtr &mat) {
    const WireFrameMaterial *material = dynamic_cast<const WireFrameMaterial *>(mat.get());
    assert( material ) ;

    setUniform("color", material->lineColor()) ;
    setUniform("width", material->lineWidth()) ;
    setUniform("fill", material->fillColor()) ;
}

}}
