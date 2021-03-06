#include <xviz/qt/scene/material.hpp>

#include <xviz/scene/material.hpp>
#include <xviz/scene/light.hpp>

#include "util.hpp"

using namespace std ;
using namespace Eigen ;


void MaterialProgram::setUniform(const char *name, float v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValue(loc, v) ;
}

void MaterialProgram::setUniform(const char *name, GLuint v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValue(loc, v) ;
}

void MaterialProgram::setUniform(const char *name, GLint v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValue(loc, v) ;
}

void MaterialProgram::setUniform(const char *name, const Vector3f &v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValueArray(loc, v.data(), 1, 3) ;
}

void MaterialProgram::setUniform(const char *name, const Vector2f &v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValueArray(loc, v.data(), 1, 2) ;
}

void MaterialProgram::setUniform(const char *name, const Vector4f &v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValueArray(loc, v.data(), 1, 4) ;
}

void MaterialProgram::setUniform(const char *name, const Matrix3f &v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValue(loc, eigenToQt(v)) ;
}

void MaterialProgram::setUniform(const char *name, const Matrix4f &v)
{
    GLint loc = prog_.uniformLocation(name) ;
    if ( loc != -1 ) prog_.setUniformValue(loc, eigenToQt(v)) ;
}

#include "shaders/common.vs.hpp"
#include "shaders/phong.fs.hpp"
#include "shaders/shadows.fs.hpp"

PhongMaterialProgram::PhongMaterialProgram(int flags): flags_(flags)
{
    std::string vs_preproc("#version 330\n") ;
    vs_preproc.append("#define HAS_NORMALS\n") ;
    if ( flags & HAS_DIFFUSE_TEXTURE ) vs_preproc.append("#define HAS_UVs\n") ;
    if ( flags & ENABLE_SKINNING ) vs_preproc.append("#define USE_SKINNING\n");
    if ( flags & ENABLE_SHADOWS ) vs_preproc.append("#define HAS_SHADOWS\n") ;

    string vs_code = vs_preproc + vertex_shader_code ;

    std::string fs_preproc("#version 330\n") ;

    if ( flags & HAS_DIFFUSE_TEXTURE )
        fs_preproc.append("#define HAS_DIFFUSE_MAP") ;

    if ( flags & ENABLE_SHADOWS )
        fs_preproc.append("#define HAS_SHADOWS\n") ;

    string fs_code = fs_preproc ;
    fs_code += phong_fragment_shader_vars ;
    if ( flags & ENABLE_SHADOWS )
        fs_code += shadows_fragment_shader ;
    fs_code += phong_fragment_shader_common ;
    fs_code += phong_fragment_shader;

    prog_.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_code.c_str()) ;
    prog_.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_code.c_str()) ;
    prog_.link() ;

}

void PhongMaterialProgram::applyParams(const xviz::MaterialPtr &mat) {
    const xviz::PhongMaterial *material = dynamic_cast<const xviz::PhongMaterial *>(mat.get());
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


void MaterialProgram::applyDefaultLight(const xviz::LightPtr &light, const Affine3f &tf, const Matrix4f &lsmat)
{
    QByteArray vname("g_light_source") ;

    if ( const auto &alight = std::dynamic_pointer_cast<xviz::AmbientLight>(light) ) {

        setUniform(vname + ".light_type", 0) ;
        setUniform(vname + ".color", alight->color_) ;
    }
    else if ( const auto &dlight = std::dynamic_pointer_cast<xviz::DirectionalLight>(light) ) {
        setUniform(vname + ".light_type", 1) ;
        setUniform(vname + ".color", dlight->diffuse_color_) ;
        setUniform(vname + ".direction", tf * (dlight->position_ - dlight->target_).normalized()) ;
    }
    else if ( const auto &slight = std::dynamic_pointer_cast<xviz::SpotLight>(light) ) {

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
    else if ( const auto &plight = std::dynamic_pointer_cast<xviz::PointLight>(light)) {
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



void MaterialProgram::use() {
    prog_.bind() ;
}

#include "shaders/constant.fs.hpp"

ConstantMaterialProgram::ConstantMaterialProgram(int flags): flags_(flags)
{
    std::string preproc("#version 330\n") ;
    if ( flags & ENABLE_SKINNING ) preproc.append("#define USE_SKINNING\n");
    if ( flags & ENABLE_SHADOWS ) preproc.append("#define HAS_SHADOWS\n") ;

    string vs_code = preproc + vertex_shader_code ;

    string fs_code = preproc + constant_fragment_shader_vars ;
    if ( flags & ENABLE_SHADOWS )
            fs_code += shadows_fragment_shader ;
    fs_code += constant_fragment_shader ;

    prog_.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_code.c_str()) ;
    prog_.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_code.c_str()) ;
    prog_.link() ;

}

void ConstantMaterialProgram::applyParams(const xviz::MaterialPtr &mat) {
    const xviz::ConstantMaterial *material = dynamic_cast<const xviz::ConstantMaterial *>(mat.get());
    assert( material ) ;

    setUniform("color", material->color()) ;
}

#include "shaders/per_vertex.fs.hpp"

PerVertexColorMaterialProgram::PerVertexColorMaterialProgram(int flags) {
    std::string preproc("#version 330\n") ;
    preproc.append("#define HAS_COLORS");
    if ( flags & ENABLE_SKINNING ) preproc.append("#define USE_SKINNING");

    string vs_code = preproc + vertex_shader_code ;
    string fs_code = preproc + per_vertex_color_fragment_shader ;

    prog_.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_code.c_str()) ;
    prog_.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_code.c_str()) ;
    prog_.link() ;

}

void PerVertexColorMaterialProgram::applyParams(const xviz::MaterialPtr &mat) {
    const xviz::PerVertexColorMaterial *material = dynamic_cast<const xviz::PerVertexColorMaterial *>(mat.get());
    assert( material ) ;

     setUniform("opacity", material->opacity()) ;
}
