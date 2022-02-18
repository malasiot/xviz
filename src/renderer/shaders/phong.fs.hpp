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
    FragColor = phongIllumination(texture(diffuseMap, vec2(map_transform * vec3(uv, 1))).rgb);
#else
    FragColor = phongIllumination(g_material.diffuse);
#endif
}
)";
