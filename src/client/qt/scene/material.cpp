#include "material.hpp"
#include <xviz/scene/material.hpp>
#include <xviz/scene/light.hpp>

using namespace std ;
using namespace Eigen ;

QMatrix4x4 eigenToQt( const Eigen::Matrix4f& transform ) {
  return QMatrix4x4(transform.data()).transposed();
}

QMatrix3x3 eigenToQt( const Matrix3f& transform ) {
  return QMatrix3x3(transform.data()).transposed();
}

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

static const char *vertex_shader_code =
        R"(
        layout (location = 0) in vec3 vposition;
        out vec3 position;

#ifdef HAS_NORMALS
        layout (location = 1) in vec3 vnormal;
        out vec3 normal;
#endif

#ifdef HAS_COLORS
        layout (location = 2) in vec3 vcolor;
        out vec3 color ;
#endif

#ifdef USE_SKINNING
        layout (location = 3) in ivec4 boneIDs;
        layout (location = 4) in vec4  boneWeights;

        const int MAX_BONES = 200;

        uniform mat4 g_bones[MAX_BONES];
#endif

#ifdef  HAS_UVs
        layout (location = 5) in vec2 vuv;
        out vec2 uv;
#endif
        uniform mat4 model ;
        uniform mat4 mvp;
        uniform mat4 mv;
        uniform mat3 mvn ;

        void main()
        {
#ifdef USE_SKINNING
        mat4 BoneTransform = g_bones[boneIDs[0]] * boneWeights[0];
        BoneTransform     += g_bones[boneIDs[1]] * boneWeights[1];
        BoneTransform     += g_bones[boneIDs[2]] * boneWeights[2];
        BoneTransform     += g_bones[boneIDs[3]] * boneWeights[3];

        vec4 posl    = BoneTransform * vec4(vposition, 1.0);

#ifdef HAS_NORMALS
        vec3 normall = mat3(BoneTransform) * vnormal;
#endif

#else
        vec4 posl = vec4(vposition, 1.0);

#ifdef HAS_NORMALS
        vec3 normall = vnormal;
#endif

#endif // SKINING

        gl_Position  = mvp * posl;

#ifdef HAS_NORMALS
        normal = mvn * normall;
#endif

#ifdef HAS_COLORS
       color = vcolor ;
#endif

#ifdef HAS_UVs
        uv = vuv ;
#endif
        position    = (mv * posl).xyz;
}
)";

static const char *phong_fragment_shader_common =
        R"(

        in vec3 normal;
        in vec3 position;

        const int MAX_LIGHTS = 10;
        const int AMBIENT_LIGHT = 0 ;
        const int DIRECTIONAL_LIGHT = 1 ;
        const int SPOT_LIGHT = 2 ;
        const int POINT_LIGHT = 3 ;

        struct LightSourceParameters
        {
        int light_type ;
        vec3 color;
        vec4 position;
        vec3 direction;
        float spot_exponent;
        float spot_cos_cutoff;
        float constant_attenuation;
        float linear_attenuation;
        float quadratic_attenuation;
        };

        uniform LightSourceParameters g_light_source[MAX_LIGHTS];

        struct MaterialParameters
        {
        vec4 ambient;     // Acm
        vec4 diffuse;     // Dcm
        vec4 specular;    // Scm
        float shininess;  // Srm
        };

        uniform MaterialParameters g_material;

     out vec4 FragColor;

     vec4 phongIllumination(vec4 dc) {
        vec3 N = normalize(normal);
        vec4 finalColor = vec4(0, 0, 0, 1.0);

        float shadow  = 0.0 ;

        for (int i=0;i<MAX_LIGHTS;i++)
        {
        vec3 L ;
        float att = 1.0 ;

        if ( g_light_source[i].light_type == -1 ) continue ;
        else if ( g_light_source[i].light_type == AMBIENT_LIGHT ) {
        finalColor += vec4(g_light_source[i].color, 1.0) * g_material.ambient ;
        continue ;
        }
        else if ( g_light_source[i].light_type == DIRECTIONAL_LIGHT ) {
        L = normalize(g_light_source[i].direction) ;
        }
        else if ( g_light_source[i].light_type == SPOT_LIGHT ) {
        float dist = length(g_light_source[i].position.xyz - position) ;
        L = normalize(g_light_source[i].position.xyz - position);

        float spotEffect = dot(normalize(g_light_source[i].direction), normalize(-L));
        if (spotEffect > g_light_source[i].spot_cos_cutoff) {
        spotEffect = pow(spotEffect, g_light_source[i].spot_exponent);
        att = spotEffect / (g_light_source[i].constant_attenuation +
        g_light_source[i].linear_attenuation * dist +
        g_light_source[i].quadratic_attenuation * dist * dist);

        }
        else att = 0.0 ;
        }
        else if ( g_light_source[i].light_type == POINT_LIGHT ) {
        float dist = length(g_light_source[i].position.xyz - position);
        L = normalize(g_light_source[i].position.xyz - position);

        att = 1.0 / (g_light_source[i].constant_attenuation +
        g_light_source[i].linear_attenuation * dist +
        g_light_source[i].quadratic_attenuation * dist * dist);

        }

        vec3 E = normalize(-position); // we are in Eye Coordinates, so EyePos is (0,0,0)
        vec3 R = normalize(-reflect(L,N));

        //calculate Diffuse Term:

        vec4 Idiff = vec4(g_light_source[i].color, 1.0) * dc * max(dot(N,L), 0.0f);
        Idiff = clamp(Idiff, 0.0, 1.0);

        // calculate Specular Term:
        vec4 Ispec = vec4(g_light_source[i].color, 1.0) * g_material.specular
        * pow(max(dot(R,E),0.0f),g_material.shininess);
        Ispec = clamp(Ispec, 0.0, 1.0);

        finalColor +=  att*clamp((1 - shadow)*(Ispec + Idiff), 0.0, 1.0);

        }

        return  finalColor ;
        }
        )";

static const char *phong_fragment_shader_material = R"(
                                               void main (void)
                                               {
                                               FragColor = phongIllumination(g_material.diffuse);
                                               }
                                               )";

static const char *phong_fragment_shader_map = R"(
                                          in vec2 uv ;
                                          uniform sampler2D tex_unit;

                                          void main (void)
                                          {
                                          FragColor = phongIllumination(texture(tex_unit, uv));
                                          }
                                          )";


PhongMaterialProgram::PhongMaterialProgram(int flags): flags_(flags)
{
    std::string preproc("#version 330\n") ;
    preproc.append("#define HAS_NORMALS\n") ;
    if ( flags & HAS_DIFFUSE_TEXTURE ) preproc.append("#define HAS_UVs\n") ;

    string vs_code = preproc + vertex_shader_code ;

    string fs_code = preproc + phong_fragment_shader_common +
            (( flags & HAS_DIFFUSE_TEXTURE ) ? phong_fragment_shader_map : phong_fragment_shader_material);


    prog_.addShaderFromSourceCode(QOpenGLShader::Vertex, vs_code.c_str()) ;
    prog_.addShaderFromSourceCode(QOpenGLShader::Fragment, fs_code.c_str()) ;
    prog_.link() ;

}

void PhongMaterialProgram::applyParams(const xviz::MaterialPtr &mat)
{
    const xviz::PhongMaterial *material = dynamic_cast<const xviz::PhongMaterial *>(mat.get());
    if ( material == nullptr ) return ;

    setUniform("g_material.ambient", material->ambientColor()) ;
    setUniform("g_material.specular", material->specularColor()) ;
    setUniform("g_material.shininess", material->shininess());
    setUniform("g_material.diffuse", material->diffuseColor());

    if ( flags_ & HAS_DIFFUSE_TEXTURE ) {
        setUniform("tex_unit", 0) ;
        // read and upload texture
    }
}


void MaterialProgram::applyBoneTransform(uint idx, const Matrix4f &tf)
{
    ostringstream name ;
    name << "g_bones[" << idx << "]" ;
    setUniform(name.str().c_str(), tf) ;
}

void MaterialProgram::applyDefaultPerspective(const Matrix4f &cam, const Matrix4f &view, const Matrix4f &model)
{
    Matrix4f mvp =  cam * view * model;
    Matrix4f mv =   view * model;

    Matrix3f wpi = mv.block<3, 3>(0, 0).transpose().eval() ;
    Matrix3f wp(wpi.inverse().eval()) ;

    setUniform("mvp", mvp) ;
    setUniform("mv", mv) ;
    setUniform("mvn", wp) ;
    setUniform("model", model) ;
}


void MaterialProgram::applyDefaultLight(uint light_index, const xviz::LightPtr &light, const Affine3f &tf)
{
    QByteArray vname = QString("g_light_source[%1]").arg(light_index ++).toLatin1() ;

    if ( const auto &alight = std::dynamic_pointer_cast<xviz::AmbientLight>(light) ) {

        setUniform(vname + ".light_type", 0) ;
        setUniform(vname + ".color", alight->color_) ;
    }
    else if ( const auto &dlight = std::dynamic_pointer_cast<xviz::DirectionalLight>(light) ) {
        setUniform(vname + ".light_type", 1) ;
        setUniform(vname + ".color", dlight->diffuse_color_) ;
        setUniform(vname + ".direction", tf * dlight->direction_) ;
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

}


void MaterialProgram::use() {
    prog_.bind() ;
}

