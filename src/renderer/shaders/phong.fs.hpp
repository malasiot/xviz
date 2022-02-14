
static const char *phong_fragment_shader_vars = R"(
in vec3 normal;
in vec3 position;

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
float diffuse(LightSourceParameters ls, vec3 N, vec3 L) {
     return  max(dot(N,L), 0.0f);
}

float specular(LightSourceParameters ls, vec3 N, vec3 L) {
     vec3 E = normalize(-position); // we are in Eye Coordinates, so EyePos is (0,0,0)
     vec3 R = normalize(-reflect(L,N));
     return pow(max(dot(R,E),0.0f), g_material.shininess);
}

vec4 phong(LightSourceParameters ls, vec4 dc, vec3 N, vec3 L, float shadow, float att) {
    float diff = diffuse(ls, N, L) ;
    float spec = specular(ls, N, L) ;

    vec4 color = vec4(ls.color, 1.0) ;
    vec4 Iamb = color * g_material.ambient ;
    vec4 Idiff = color * dc * diff;
    vec4 Ispec = color * g_material.specular * spec ;

    return att * ((1 - shadow)*(Ispec + Idiff) + Iamb) ;
}

vec4 phongDirectional(LightSourceParameters ls, vec4 dc, vec3 N, float shadow) {
    vec3 L = normalize(ls.direction) ;
    return phong(ls, dc, N, L, shadow, 1.0) ;
}

vec4 phongSpot(LightSourceParameters ls, vec4 dc, vec3 N, float shadow) {
    vec3 L = normalize(ls.position.xyz - position);
    float att = 0.0 ;
    float dist = length(ls.position.xyz - position) ;
    float spotEffect = dot(normalize(ls.direction), normalize(-L));
    if (spotEffect > ls.spot_cos_cutoff) {
       spotEffect = pow(spotEffect, ls.spot_exponent);
       att = spotEffect / (ls.constant_attenuation +
           ls.linear_attenuation * dist +
           ls.quadratic_attenuation * dist * dist);
    }

    return phong(ls, dc, N, L, shadow, att) ;
}

vec4 phongPoint(LightSourceParameters ls, vec4 dc, vec3 N, float shadow) {
    vec3 L = normalize(ls.position.xyz - position);
    float dist = length(ls.position.xyz - position);

    float att = 1.0 / (ls.constant_attenuation +
           ls.linear_attenuation * dist +
           ls.quadratic_attenuation * dist * dist);

    return phong(ls, dc, N, L, shadow, att) ;
}

vec4 phongIllumination(vec4 dc) {
    vec3 N = normalize(normal);
    vec4 finalColor = vec4(0, 0, 0, 1.0);

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_DIRECTIONAL_LIGHTS ; i++ ) {
        finalColor += phongDirectional(g_light_source_dir[i], dc, N, 0.0) ;
    }
#pragma unroll_loop_end

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW ; i++ ) {
        float shadow = calcShadow(lspos_d[i], g_light_source_dir_shadow[i].shadowMap, g_light_source_dir_shadow[i].shadowBias);
        finalColor += phongDirectional(g_light_source_dir_shadow[i], dc, N, shadow) ;
    }
#pragma unroll_loop_end

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_SPOT_LIGHTS ; i++ ) {
        finalColor += phongSpot(g_light_source_spot[i], dc, N, 0.0) ;
    }
#pragma unroll_loop_end

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_SPOT_LIGHTS_WITH_SHADOW ; i++ ) {
        float shadow = calcShadow(lspos_s[i], g_light_source_spot_shadow[i].shadowMap, g_light_source_spot_shadow[i].shadowBias);
        finalColor += phongSpot(g_light_source_spot_shadow[i], dc, N, shadow) ;
    }
#pragma unroll_loop_end

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_POINT_LIGHTS ; i++ ) {
        finalColor += phongPoint(g_light_source_point[i], dc, N, 0.0) ;
    }
#pragma unroll_loop_end

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_POINT_LIGHTS_WITH_SHADOW ; i++ ) {
        float shadow = calcShadow(lspos_p[i], g_light_source_point_shadow[i].shadowMap, g_light_source_point_shadow[i].shadowBias);
        finalColor += phongPoint(g_light_source_point_shadow[i], dc, N, shadow) ;
    }
#pragma unroll_loop_end

    return  clamp(finalColor, 0.0, 1.0) ;
}
)";


static const char *phong_fragment_shader = R"(
#version 330

#include <@phong_fragment_shader_vars>
#include <@light_vars>
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
