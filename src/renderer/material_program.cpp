#include "material_program.hpp"

#include <xviz/scene/material.hpp>
#include <xviz/scene/light.hpp>
#include "shadow_map.hpp"
#include "util.hpp"
#include "texture_data.hpp"
#include "renderer_impl.hpp"

using namespace std ;
using namespace Eigen ;

namespace xviz { namespace impl {

PhongMaterialProgram::PhongMaterialProgram(const Params &params): params_(params)
{
    OpenGLShaderPreproc vs_preproc ;

    vs_preproc.appendDefinition("HAS_SHADOWS", params.enable_shadows_) ;
    vs_preproc.appendDefinition("HAS_NORMALS") ;
    vs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS", std::to_string(params.num_dir_lights_)) ;
    vs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW", std::to_string(params.num_dir_lights_shadow_), params.enable_shadows_) ;
    vs_preproc.appendConstant("NUM_SPOT_LIGHTS", std::to_string(params.num_spot_lights_)) ;
    vs_preproc.appendConstant("NUM_SPOT_LIGHTS_WITH_SHADOW", std::to_string(params.num_spot_lights_shadow_), params.enable_shadows_) ;
    vs_preproc.appendConstant("NUM_POINT_LIGHTS", std::to_string(params.num_point_lights_)) ;
    vs_preproc.appendConstant("NUM_POINT_LIGHTS_WITH_SHADOW", std::to_string(params.num_point_lights_shadow_), params.enable_shadows_) ;
    vs_preproc.appendDefinition("HAS_UVs", params.has_diffuse_map_) ;
    vs_preproc.appendDefinition("USE_SKINNING", params.enable_skinning_);

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", vs_preproc) ;

    OpenGLShaderPreproc fs_preproc ;

    fs_preproc.appendDefinition("HAS_DIFFUSE_MAP", params.has_diffuse_map_) ;
    fs_preproc.appendDefinition("HAS_SHADOWS", params.enable_shadows_) ;
    fs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS", std::to_string(params.num_dir_lights_)) ;
    fs_preproc.appendConstant("NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW", std::to_string(params.num_dir_lights_shadow_), params.enable_shadows_) ;
    fs_preproc.appendConstant("NUM_SPOT_LIGHTS", std::to_string(params.num_spot_lights_)) ;
    fs_preproc.appendConstant("NUM_SPOT_LIGHTS_WITH_SHADOW", std::to_string(params.num_spot_lights_shadow_), params.enable_shadows_) ;
    fs_preproc.appendConstant("NUM_POINT_LIGHTS", std::to_string(params.num_point_lights_)) ;
    fs_preproc.appendConstant("NUM_POINT_LIGHTS_WITH_SHADOW", std::to_string(params.num_point_lights_shadow_), params.enable_shadows_) ;

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

    if ( params_.has_diffuse_map_ & HAS_DIFFUSE_TEXTURE ) {
        setUniform("diffuseMap", 0) ;
    }
}

void MaterialProgram::bindTexture(const Texture2D *texture, TextureLoader loader, int slot) {
    if ( !texture ) return ;

    impl::TextureData *data = loader(texture) ;

    if ( data && data->loaded() ) {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, data->id());

        setUniform("map_transform", texture->transform().matrix()) ;
    }
}

void PhongMaterialProgram::bindTextures(const MaterialPtr &mat, TextureLoader loader)
{
    const PhongMaterial *m = dynamic_cast<const PhongMaterial *>(mat.get()) ;
    assert(m) ;

    bindTexture(m->diffuseTexture(), loader, 0) ;
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
    setUniform("eyePos", Vector4f(view.inverse() * Vector4f(0, 0, 0, 1)));
}


void MaterialProgram::applyDirectionalLight(GLuint idx, const LightPtr &light, const Affine3f &tf, const Matrix4f &lsmat, GLuint tindex) {
    const auto &dlight = std::dynamic_pointer_cast<DirectionalLight>(light) ;

    string vname("g_light_source_dir") ;
    vname += ( light->castsShadows() ? "_shadow[" : "[" ) + std::to_string(idx) + "]" ;

    setUniform(vname + ".ambient", dlight->ambientColor()) ;
    setUniform(vname + ".diffuse", dlight->diffuseColor()) ;
    setUniform(vname + ".specular", dlight->specularColor()) ;

    setUniform(vname + ".direction", tf * (dlight->position() - dlight->target()).normalized()) ;

    if ( light->castsShadows() ) {
        setUniform("lsmat_d[" + std::to_string(idx) + "]", Matrix4f(lsmat)) ;
        setUniform(vname + ".shadowMap", (GLuint)(4+tindex)) ;
        setUniform(vname + ".shadowBias", light->shadowBias()) ;
    }
}

void MaterialProgram::applySpotLight(GLuint idx, const LightPtr &light, const Affine3f &tf, const Matrix4f &lsmat, GLuint tindex) {
    const auto &slight = std::dynamic_pointer_cast<SpotLight>(light);

    string vname("g_light_source_spot") ;
    vname += ( light->castsShadows() ? "_shadow[" : "[" ) + std::to_string(idx) + "]" ;

    setUniform(vname + ".ambient", slight->ambientColor()) ;
    setUniform(vname + ".diffuse", slight->diffuseColor()) ;
    setUniform(vname + ".specular", slight->specularColor()) ;
    setUniform(vname + ".direction", slight->direction()) ;
    setUniform(vname + ".position", tf * slight->position()) ;
    setUniform(vname + ".constant_attenuation", slight->constantAttenuation()) ;
    setUniform(vname + ".linear_attenuation", slight->linearAttenuation()) ;
    setUniform(vname + ".quadratic_attenuation", slight->quadraticAttenuation()) ;

    setUniform(vname + ".spot_inner_cutoff", (float)cos(M_PI*slight->innerCutoffAngle()/180.0)) ;
    setUniform(vname + ".spot_outer_cutoff", (float)cos(M_PI*slight->outerCutoffAngle()/180.0)) ;

    if ( light->castsShadows() ) {
        setUniform("lsmat_s[" + std::to_string(idx) + "]", Matrix4f(lsmat)) ;
        setUniform(vname + ".shadowMap", 4+(GLuint)(4+tindex)) ;
        setUniform(vname + ".shadowBias", light->shadowBias()) ;
    }

}

void MaterialProgram::applyPointLight(GLuint idx, const LightPtr &light, const Affine3f &tf, const Matrix4f &lsmat, GLuint tindex) {
    const auto &plight = std::dynamic_pointer_cast<PointLight>(light);

    string vname("g_light_source_point") ;
    vname += ( light->castsShadows() ? "_shadow[" : "[" ) + std::to_string(idx) + "]" ;

    setUniform(vname + ".ambient", plight->ambientColor()) ;
    setUniform(vname + ".diffuse", plight->diffuseColor()) ;
    setUniform(vname + ".specular", plight->specularColor()) ;

    setUniform(vname + ".position", tf * plight->position()) ;
    setUniform(vname + ".constant_attenuation", plight->constantAttenuation()) ;
    setUniform(vname + ".linear_attenuation", plight->linearAttenuation()) ;
    setUniform(vname + ".quadratic_attenuation", plight->quadraticAttenuation()) ;

    if ( light->castsShadows() ) {
        setUniform("lsmat_p[" + std::to_string(idx) + "]", Matrix4f(lsmat)) ;
        setUniform(vname + ".shadowMap", (GLuint)(4+tindex)) ;
        setUniform(vname + ".shadowBias", light->shadowBias()) ;
    }
}

void MaterialProgram::applyDefaultLights(const std::vector<LightData *> &lights) {

    size_t i, k, t=0 ;
    for( i=0, k=0 ; i<lights.size() ; i++ ) {
        const auto &ld = lights[i] ;
        const auto &light = ld->light_ ;

        if ( dynamic_cast<const DirectionalLight *>(light.get()) && !light->castsShadows() ) {
            applyDirectionalLight(k++, ld->light_, ld->mat_, ld->ls_mat_, 0) ;
        }
    }

    for( i=0, k=0 ; i<lights.size() ; i++ ) {
        const auto &ld = lights[i] ;
        const auto &light = ld->light_ ;

        if ( dynamic_cast<const DirectionalLight *>(light.get()) && light->castsShadows() ) {
            ld->shadow_map_->bindTexture(GL_TEXTURE0 + 4 + t) ;
            applyDirectionalLight(k++, ld->light_, ld->mat_, ld->ls_mat_, t) ;
            ++t ;
        }
    }

    for( i=0, k=0 ; i<lights.size() ; i++ ) {
        const auto &ld = lights[i] ;
        const auto &light = ld->light_ ;

        if ( dynamic_cast<const SpotLight *>(light.get()) && !light->castsShadows() ) {
            applySpotLight(k++, ld->light_, ld->mat_, ld->ls_mat_, 0) ;
        }
    }

    for( i=0, k=0 ; i<lights.size() ; i++ ) {
        const auto &ld = lights[i] ;
        const auto &light = ld->light_ ;

        if ( dynamic_cast<const SpotLight *>(light.get()) && light->castsShadows() ) {
            ld->shadow_map_->bindTexture(GL_TEXTURE0 + 4 + t) ;
            applySpotLight(k++, ld->light_, ld->mat_, ld->ls_mat_, t) ;
            ++t ;
        }
    }

    for( i=0, k=0 ; i<lights.size() ; i++ ) {
        const auto &ld = lights[i] ;
        const auto &light = ld->light_ ;

        if ( dynamic_cast<const PointLight *>(light.get()) && !light->castsShadows() ) {
            applyPointLight(k++, ld->light_, ld->mat_, ld->ls_mat_, 0) ;
        }
    }

    for( i=0, k=0 ; i<lights.size() ; i++ ) {
        const auto &ld = lights[i] ;
        const auto &light = ld->light_ ;

        if ( dynamic_cast<const PointLight *>(light.get()) && light->castsShadows() ) {
            ld->shadow_map_->bindTexture(GL_TEXTURE0 + 4 + t) ;
            applyPointLight(k++, ld->light_, ld->mat_, ld->ls_mat_, t) ;
            ++t ;
        }
    }

}

ConstantMaterialProgram::ConstantMaterialProgram(const Params &params): params_(params) {
    OpenGLShaderPreproc preproc ;

    preproc.appendDefinition("USE_SKINNING", params.enable_skinning_);

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@constant_fragment_shader", preproc) ;

    link() ;
}

void ConstantMaterialProgram::applyParams(const MaterialPtr &mat) {
    const ConstantMaterial *material = dynamic_cast<const ConstantMaterial *>(mat.get());
    assert( material ) ;

    setUniform("color", material->color()) ;
}

PerVertexColorMaterialProgram::PerVertexColorMaterialProgram(const Params &params) {

    OpenGLShaderPreproc preproc ;
    preproc.appendDefinition("HAS_COLORS");

    preproc.appendDefinition("USE_SKINNING", params.enable_skinning_);

    addShaderFromFile(VERTEX_SHADER, "@vertex_shader", preproc) ;
    addShaderFromFile(FRAGMENT_SHADER, "@per_vertex_color_fragment_shader", preproc) ;

    link() ;
}

void PerVertexColorMaterialProgram::applyParams(const MaterialPtr &mat) {
    const PerVertexColorMaterial *material = dynamic_cast<const PerVertexColorMaterial *>(mat.get());
    assert( material ) ;

    setUniform("opacity", material->opacity()) ;
}


WireFrameMaterialProgram::WireFrameMaterialProgram(const Params &params) {

    OpenGLShaderPreproc preproc ;

    preproc.appendDefinition("USE_SKINNING", params.enable_skinning_);

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

string PhongMaterialProgram::Params::key() const {
    std::stringstream strm ;
    strm << num_dir_lights_ << ',' ;
    strm << num_dir_lights_shadow_ << ',' ;
    strm << num_point_lights_ << ',' ;
    strm << num_point_lights_shadow_ << ',' ;
    strm << num_spot_lights_ << ',' ;
    strm << num_spot_lights_shadow_ << ',' ;
    strm << (int)enable_shadows_ << ',' ;
    strm << (int)enable_skinning_ << ',' ;
    strm << (int)has_diffuse_map_ << ',' ;

    return strm.str() ;
}

string ConstantMaterialProgram::Params::key() const {
    std::stringstream strm ;
    strm << (int)enable_skinning_ ;
    return strm.str() ;
}

string PerVertexColorMaterialProgram::Params::key() const {
    std::stringstream strm ;
    strm << (int)enable_skinning_ ;
    return strm.str() ;
}

string WireFrameMaterialProgram::Params::key() const {
    std::stringstream strm ;
    strm << (int)enable_skinning_ ;
    return strm.str() ;
}
}}
