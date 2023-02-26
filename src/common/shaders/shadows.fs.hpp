static const char *shadows_fragment_shader = R"(
#ifdef HAS_SHADOWS

#if NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW > 0
in vec4 lspos_d[NUM_DIRECTIONAL_LIGHTS_WITH_SHADOW] ;
#endif

#if NUM_SPOT_LIGHTS_WITH_SHADOW > 0
in vec4 lspos_s[NUM_SPOT_LIGHTS_WITH_SHADOW] ;
#endif

#if NUM_POINT_LIGHTS_WITH_SHADOW > 0
in vec4 lspos_p[NUM_POINT_LIGHTS_WITH_SHADOW] ;
#endif

float calcShadow(vec4 fragPosLightSpace, sampler2DShadow shadowMap, float shadowBias) {

// perform perspective divide
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

   // transform to [0,1] range
   projCoords = projCoords * 0.5 + 0.5;
   projCoords.z -= shadowBias ;

   float currentDepth = projCoords.z;
   if (projCoords.z > 1.0)
      return 0.0;

   float d = texture(shadowMap, projCoords) ;
   float shadow = 1 - d ;
   return shadow ;
}
#endif
)";
