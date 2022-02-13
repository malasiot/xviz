static const char *constant_fragment_shader_vars = R"(
uniform vec4 color ;
uniform bool light_casts_shadows ;
)";

static const char *constant_fragment_shader = R"(
#version 330

#include <@constant_fragment_shader_vars>
#include <@light_vars>
#include <@shadows_fragment_shader>

out vec4 FragColor;

void main (void) {
float shadow = 0.0;
#ifdef HAS_SHADOWS
        for(int i=0 ;i<NUM_LIGHTS ; i++ ) {
            if ( g_light_source[i].light_casts_shadows ) {
                shadow += calcShadow(lspos[i], g_light_source[i].shadowMap, g_light_source[i].shadowBias);
            }
        }

        shadow = clamp(shadow, 0.0, 1.0) ;
#endif
    FragColor = color * vec4(1-shadow, 1-shadow, 1-shadow, 1.0);
}
)";
