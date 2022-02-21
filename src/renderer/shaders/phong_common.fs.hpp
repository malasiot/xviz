


static const char *phong_fragment_shader_common = R"(
float diffuse(LightSourceParameters ls, vec3 N, vec3 L) {
     return  max(dot(N,L), 0.0f);
}

float specular(LightSourceParameters ls, vec3 N, vec3 L) {
     vec3 E = normalize(eyePos-fpos);
     vec3 R = normalize(reflect(-L,N));
     return pow(max(dot(R,E),0.0f), max(g_material.shininess, 0.1));
}

vec3 phong(LightSourceParameters ls, vec3 dc, vec3 N, vec3 L, float shadow, float att) {
    float diff = diffuse(ls, N, L) ;
    float spec = specular(ls, N, L) ;

    vec3 Iamb = ls.ambient * g_material.ambient ;
    vec3 Idiff = ls.diffuse * dc * diff;
    vec3 Ispec = ls.specular * g_material.specular * spec ;

    return att * ((1 - shadow)*(Ispec + Idiff)) + Iamb ;
}

vec3 phongDirectional(LightSourceParameters ls, vec3 dc, vec3 N, float shadow) {
    vec3 L = normalize(ls.direction) ;
    return phong(ls, dc, N, L, shadow, 1.0) ;
}

vec3 phongSpot(LightSourceParameters ls, vec3 dc, vec3 N, float shadow) {
    vec3 L = normalize(ls.position.xyz - fpos);
    float dist = length(ls.position.xyz - fpos) ;
    float theta = dot(normalize(-ls.direction), L);
    float epsilon = (ls.spot_inner_cutoff - ls.spot_outer_cutoff);
    float intensity = clamp((theta - ls.spot_outer_cutoff) / epsilon, 0.0, 1.0);
    float att = intensity / (ls.constant_attenuation +
           ls.linear_attenuation * dist +
           ls.quadratic_attenuation * dist * dist);

  return phong(ls, dc, N, L, shadow, att) ;
}

vec3 phongPoint(LightSourceParameters ls, vec3 dc, vec3 N, float shadow) {
    vec3 L = normalize(ls.position.xyz - fpos);
    float dist = length(ls.position.xyz - fpos);

    float att = 1.0 / (ls.constant_attenuation +
           ls.linear_attenuation * dist +
           ls.quadratic_attenuation * dist * dist);

    return phong(ls, dc, N, L, shadow, att) ;
}

vec4 phongIllumination(vec3 dc) {
    vec3 N = normalize(normal);
    vec3 finalColor = vec3(0);

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_DIRECTIONAL_LIGHTS ; i++ ) {
        finalColor += phongDirectional(g_light_source_dir[i], dc, N, 0.0) ;
    }
#pragma unroll_loop_end

#pragma unroll_loop_start
    for( int i=0 ; i<NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW ; i++ ) {
        float shadow = calcShadow(lspos_d[i], g_light_source_dir_shadow[i].shadow_map, g_light_source_dir_shadow[i].shadow_bias);
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
        float shadow = calcShadow(lspos_s[i], g_light_source_spot_shadow[i].shadow_map, g_light_source_spot_shadow[i].shadow_bias);
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
        float shadow = calcShadow(lspos_p[i], g_light_source_point_shadow[i].shadow_map, g_light_source_point_shadow[i].shadow_bias);
        finalColor += phongPoint(g_light_source_point_shadow[i], dc, N, shadow) ;
    }
#pragma unroll_loop_end

    return  vec4(clamp(finalColor, 0.0, 1.0), g_material.opacity) ;
}
)";

