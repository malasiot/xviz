
static const char *phong_fragment_shader_vars = R"(
in vec3 normal;
in vec3 position;

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

uniform bool light_casts_shadows ;
uniform LightSourceParameters g_light_source;

struct MaterialParameters
{
    vec4 ambient;     // Acm
    vec4 diffuse;     // Dcm
    vec4 specular;    // Scm
    float shininess;  // Srm
    vec4 emission;
};

uniform MaterialParameters g_material;
out vec4 FragColor;
)";


static const char *phong_fragment_shader_common = R"(

vec4 phongIllumination(vec4 dc) {
    vec3 N = normalize(normal);
    vec4 finalColor = vec4(0, 0, 0, 1.0);

    float shadow  = 0.0 ;

    vec3 L ;
    float att = 1.0 ;

    if ( g_light_source.light_type == -1 )  ;
    else if ( g_light_source.light_type == AMBIENT_LIGHT ) {
        return vec4(g_light_source.color, 1.0) * g_material.ambient ;
    }
    else if ( g_light_source.light_type == DIRECTIONAL_LIGHT ) {
        L = normalize(g_light_source.direction) ;
    }
    else if ( g_light_source.light_type == SPOT_LIGHT ) {
        float dist = length(g_light_source.position.xyz - position) ;
        L = normalize(g_light_source.position.xyz - position);

        float spotEffect = dot(normalize(g_light_source.direction), normalize(-L));
        if (spotEffect > g_light_source.spot_cos_cutoff) {
            spotEffect = pow(spotEffect, g_light_source.spot_exponent);
            att = spotEffect / (g_light_source.constant_attenuation +
            g_light_source.linear_attenuation * dist +
            g_light_source.quadratic_attenuation * dist * dist);
        }
        else att = 0.0 ;
    }
    else if ( g_light_source.light_type == POINT_LIGHT ) {
        float dist = length(g_light_source.position.xyz - position);
        L = normalize(g_light_source.position.xyz - position);

        att = 1.0 / (g_light_source.constant_attenuation +
        g_light_source.linear_attenuation * dist +
        g_light_source.quadratic_attenuation * dist * dist);
    }

#ifdef HAS_SHADOWS
    if ( light_casts_shadows ) {
        shadow = calcShadow(lspos, shadowBias);
    }
#endif

    vec3 E = normalize(-position); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L,N));

    vec4 Iamb = vec4(g_light_source.color, 1.0) * g_material.ambient ;

    //calculate Diffuse Term:

    vec4 Idiff = vec4(g_light_source.color, 1.0) * dc * max(dot(N,L), 0.0f);
    Idiff = clamp(Idiff, 0.0, 1.0);

    // calculate Specular Term:
    vec4 Ispec = vec4(g_light_source.color, 1.0) * g_material.specular
        * pow(max(dot(R,E),0.0f), g_material.shininess);
    Ispec = clamp(Ispec, 0.0, 1.0);

    vec4 I = att*(Ispec + Idiff + Iamb) ;

    finalColor =  clamp((1 - shadow)*I, 0.0, 1.0);

    return  finalColor ;
}
)";

static const char *phong_fragment_shader = R"(
#version 330

#include <@phong_fragment_shader_vars>
#include <@shadows_fragment_shader>
#include <@phong_fragment_shader_common>

#ifdef HAS_DIFFUSE_MAP
in vec2 uv ;
uniform sampler2D diffuseMap;
#endif

void main (void) {
#ifdef HAS_DIFFUSE_MAP
    FragColor = phongIllumination(texture(diffuseMap, uv));
#else
    FragColor = phongIllumination(g_material.diffuse);
#endif
}
)";
