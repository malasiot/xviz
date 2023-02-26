#pragma once

static const char *light_vars = R"(

struct LightSourceParameters
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    vec3 direction;
    float spot_exponent;
    float spot_inner_cutoff;
    float spot_outer_cutoff;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;

    sampler2DShadow shadow_map ;
    float shadow_bias ;
};

#if NUM_DIRECTIONAL_LIGHTS > 0
uniform LightSourceParameters g_light_source_dir[NUM_DIRECTIONAL_LIGHTS];
#endif

#if NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW > 0
uniform LightSourceParameters g_light_source_dir_shadow[NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW];
#endif

#if NUM_SPOT_LIGHTS > 0
uniform LightSourceParameters g_light_source_spot[NUM_SPOT_LIGHTS];
#endif

#if NUM_SPOT_LIGHTS_WITH_SHADOW > 0
uniform LightSourceParameters g_light_source_spot_shadow[NUM_SPOT_LIGHTS_WITH_SHADOW];
#endif

#if NUM_POINT_LIGHTS > 0
uniform LightSourceParameters g_light_source_point[NUM_POINT_LIGHTS];
#endif

#if NUM_POINT_LIGHTS_WITH_SHADOW > 0
uniform LightSourceParameters g_light_source_point_shadow[NUM_POINT_LIGHTS_WITH_SHADOW];
#endif
        )";
