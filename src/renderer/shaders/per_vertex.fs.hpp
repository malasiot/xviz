static const char *per_vertex_color_fragment_shader = R"(
#version 330

in vec3 color ;
out vec4 FragColor;
uniform float opacity ;

void main (void)
{
    FragColor = vec4(color, opacity) ;
}
)";
