#pragma once

static const char *light_vars = R"(

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
                                     bool light_casts_shadows ;
                                     sampler2DShadow shadowMap ;
                                     float shadowBias ;
                                 };

                                 uniform LightSourceParameters g_light_source[NUM_LIGHTS];
        )";
