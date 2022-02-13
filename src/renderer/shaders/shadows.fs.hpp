static const char *shadows_fragment_shader = R"(
#ifdef HAS_SHADOWS

in vec4 lspos[NUM_LIGHTS] ;

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
