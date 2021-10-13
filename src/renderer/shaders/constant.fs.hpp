static const char *constant_fragment_shader_vars = R"(
uniform vec4 color ;
uniform bool light_casts_shadows ;
)";

static const char *constant_fragment_shader = R"(
#version 330

#include <@constant_fragment_shader_vars>
#include <@shadows_fragment_shader>

out vec4 FragColor;

void main (void) {
float shadow = 0.0;
#ifdef HAS_SHADOWS
        if ( light_casts_shadows ) {
            shadow = calcShadow(lspos, shadowBias);
        }
#endif
    FragColor = color * (1-shadow);
}
)";
