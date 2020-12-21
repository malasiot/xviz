
static const char *constant_fragment_shader = R"(
out vec4 FragColor;
uniform vec4 color ;
void main (void) {
    FragColor = color;
}
)";
