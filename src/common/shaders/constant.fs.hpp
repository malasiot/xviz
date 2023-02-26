static const char *constant_fragment_shader_vars = R"(
uniform vec4 color ;
uniform bool light_casts_shadows ;

#ifdef HAS_DIFFUSE_MAP
in vec2 uv ;
uniform sampler2D diffuseMap;
uniform mat3x3 map_transform ;
#endif
)";

static const char *constant_fragment_shader = R"(
#version 330

#include <@constant_fragment_shader_vars>
#include <@light_vars>
#include <@shadows_fragment_shader>

out vec4 FragColor;

void main (void) {
#ifdef HAS_DIFFUSE_MAP
FragColor = texture(diffuseMap, vec2(map_transform * vec3(uv, 1)));
#else
FragColor = color ;
#endif
}
)";
