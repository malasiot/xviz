#include "material_program.hpp"

#include <xviz/scene/material.hpp>
#include <xviz/scene/light.hpp>

#include "util.hpp"

using namespace std ;
using namespace Eigen ;

namespace xviz { namespace impl {

PhongMaterialProgram::PhongMaterialProgram(const MaterialInstanceParams &params): params_(params)
{
    OpenGLShaderPreproc vs_preproc ;

    vs_preproc.appendDefinition("HAS_SHADOWS", params.flags_ & ENABLE_SHADOWS) ;
    vs_preproc.appendDefinition("HAS_NORMALS") ;
    vs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS", std::to_string(params.num_dir_lights_)) ;
    vs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW", std::to_string(params.num_dir_lights_shadow_), params.flags_ & ENABLE_SHADOWS) ;
    vs_preproc.appendConstant("NUM_SPOT_LIGHTS", std::to_string(params.num_spot_lights_)) ;
    vs_preproc.appendConstant("NUM_SPOT_LIGHTS_WITH_SHADOW", std::to_string(params.num_spot_lights_shadow_), params.flags_ & ENABLE_SHADOWS) ;
    vs_preproc.appendConstant("NUM_POINT_LIGHTS", std::to_string(params.num_point_lights_)) ;
    vs_preproc.appendConstant("NUM_POINT_LIGHTS_WITH_SHADOW", std::to_string(params.num_point_lights_shadow_), params.flags_ & ENABLE_SHADOWS) ;
    vs_preproc.appendDefinition("HAS_UVs", params.flags_ & HAS_DIFFUSE_TEXTURE) ;
    vs_preproc.appendDefinition("USE_SKINNING", params.flags_ & ENABLE_SKINNING);

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", vs_preproc) ;

    OpenGLShaderPreproc fs_preproc ;

    fs_preproc.appendDefinition("HAS_DIFFUSE_MAP", params.flags_ & HAS_DIFFUSE_TEXTURE) ;
    fs_preproc.appendDefinition("HAS_SHADOWS", params.flags_ & ENABLE_SHADOWS) ;
    fs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS", std::to_string(params.num_dir_lights_)) ;
    fs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW", std::to_string(params.num_dir_lights_shadow_), params.flags_ & ENABLE_SHADOWS) ;
    fs_preproc.appendConstant("NUM_SPOT_LIGHTS", std::to_string(params.num_spot_lights_)) ;
    fs_preproc.appendConstant("NUM_SPOT_LIGHTS_WITH_SHADOW", std::to_string(params.num_spot_lights_shadow_), params.flags_ & ENABLE_SHADOWS) ;
    fs_preproc.appendConstant("NUM_POINT_LIGHTS", std::to_string(params.num_point_lights_)) ;
    fs_preproc.appendConstant("NUM_POINT_LIGHTS_WITH_SHADOW", std::to_string(params.num_point_lights_shadow_), params.flags_ & ENABLE_SHADOWS) ;

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
    setUniform("g_material.opacity", material->opacity());

    if ( params_.flags_ & HAS_DIFFUSE_TEXTURE ) {
        setUniform("diffuseMap", 0) ;
    }
}


void MaterialProgram::applyBoneTransform(GLuint idx, const Matrix4f &tf)
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


void MaterialProgram::applyDefaultLight(uint idx, const LightPtr &light, const Affine3f &tf, const Matrix4f &lsmat, GLuint tindex)
{

    if ( const auto &dlight = std::dynamic_pointer_cast<DirectionalLight>(light) ) {
        stringstream strm ;
        strm << "g_light_source_dir" ;
        if ( light->casts_shadows_ ) strm << "_shadow" ;
        strm << "[" << idx << "]" ;
        string vname = strm.str() ;

        setUniform(vname + ".light_type", 1) ;
        setUniform(vname + ".color", dlight->diffuse_color_) ;
        setUniform(vname + ".direction", tf * (dlight->position_ - dlight->target_).normalized()) ;

        if ( light->casts_shadows_ ) {
            setUniform("lsmat_d[" + std::to_string(idx) + "]", Matrix4f(lsmat)) ;
            setUniform(vname + ".shadowMap", (GLuint)(4+tindex)) ;
            setUniform(vname + ".shadowBias", light->shadow_bias_) ;
        }
    }
    else if ( const auto &slight = std::dynamic_pointer_cast<SpotLight>(light) ) {
        stringstream strm ;
        strm << "g_light_source_spot" ;
        if ( light->casts_shadows_ ) strm << "_shadow" ;
        strm << "[" << idx << "]" ;
        string vname = strm.str() ;

        setUniform(vname + ".light_type", 2) ;
        setUniform(vname + ".color", slight->diffuse_color_) ;
        setUniform(vname + ".direction", slight->direction_) ;
        setUniform(vname + ".position", tf * slight->position_) ;
        setUniform(vname + ".constant_attenuation", slight->constant_attenuation_) ;
        setUniform(vname + ".linear_attenuation", slight->linear_attenuation_) ;
        setUniform(vname + ".quadratic_attenuation", slight->quadratic_attenuation_) ;

        setUniform(vname + ".spot_inner_cutoff", (float)cos(M_PI*slight->inner_cutoff_angle_/180.0)) ;
        setUniform(vname + ".spot_outer_cutoff", (float)cos(M_PI*slight->outer_cutoff_angle_/180.0)) ;

        if ( light->casts_shadows_ ) {
            setUniform("lsmat_s[" + std::to_string(idx) + "]", Matrix4f(lsmat)) ;
            setUniform(vname + ".shadowMap", 4+(GLuint)(4+tindex)) ;
            setUniform(vname + ".shadowBias", light->shadow_bias_) ;
        }

    }
    else if ( const auto &plight = std::dynamic_pointer_cast<PointLight>(light)) {
        stringstream strm ;
        strm << "g_light_source_point" ;
        if ( light->casts_shadows_ ) strm << "_shadow" ;
        strm << "[" << idx << "]" ;
        string vname = strm.str() ;

        setUniform(vname + ".light_type", 3) ;
        setUniform(vname + ".color", plight->diffuse_color_) ;
        setUniform(vname + ".position", tf * plight->position_) ;
        setUniform(vname + ".constant_attenuation", plight->constant_attenuation_) ;
        setUniform(vname + ".linear_attenuation", plight->linear_attenuation_) ;
        setUniform(vname + ".quadratic_attenuation", plight->quadratic_attenuation_) ;

        if ( light->casts_shadows_ ) {
            setUniform("lsmat_p[" + std::to_string(idx) + "]", Matrix4f(lsmat)) ;
            setUniform(vname + ".shadowMap", (GLuint)(4+tindex)) ;
            setUniform(vname + ".shadowBias", light->shadow_bias_) ;
        }
    }

}

ConstantMaterialProgram::ConstantMaterialProgram(const MaterialInstanceParams &params): params_(params) {
    OpenGLShaderPreproc preproc ;

     //preproc.appendConstant("NUM_LIGHTS", std::to_string(params.num_lights_)) ;

    if ( params.flags_ & ENABLE_SKINNING ) preproc.appendDefinition("USE_SKINNING");
    if ( params.flags_ & ENABLE_SHADOWS ) preproc.appendDefinition("HAS_SHADOWS") ;

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@constant_fragment_shader", preproc) ;

    link() ;
}

void ConstantMaterialProgram::applyParams(const MaterialPtr &mat) {
    const ConstantMaterial *material = dynamic_cast<const ConstantMaterial *>(mat.get());
    assert( material ) ;

    setUniform("color", material->color()) ;
}

PerVertexColorMaterialProgram::PerVertexColorMaterialProgram(const MaterialInstanceParams &params) {

    OpenGLShaderPreproc preproc ;
    preproc.appendDefinition("HAS_COLORS");
//    preproc.appendConstant("NUM_LIGHTS", std::to_string(params.num_lights_)) ;

    if ( params.flags_ & ENABLE_SKINNING ) preproc.appendDefinition("USE_SKINNING");

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@per_vertex_color_fragment_shader", preproc) ;

    link() ;
}

void PerVertexColorMaterialProgram::applyParams(const MaterialPtr &mat) {
    const PerVertexColorMaterial *material = dynamic_cast<const PerVertexColorMaterial *>(mat.get());
    assert( material ) ;

     setUniform("opacity", material->opacity()) ;
}


WireFrameMaterialProgram::WireFrameMaterialProgram(const MaterialInstanceParams &params) {

    OpenGLShaderPreproc preproc ;
//    preproc.appendConstant("NUM_LIGHTS", std::to_string(params.num_lights_)) ;

    if ( params.flags_ & ENABLE_SKINNING ) preproc.appendDefinition("USE_SKINNING");

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

string MaterialInstanceParams::key() const {
    std::stringstream strm ;
    strm << num_dir_lights_ << ',' ;
    strm << num_dir_lights_shadow_ << ',' ;
    strm << num_point_lights_ << ',' ;
    strm << num_point_lights_shadow_ << ',' ;
    strm << num_spot_lights_ << ',' ;
    strm << num_spot_lights_shadow_ << ',' ;

    return strm.str() ;
}

}}
