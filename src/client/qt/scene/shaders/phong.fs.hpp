
static const char *phong_fragment_shader = R"(
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
