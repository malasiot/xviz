static const char *phong_fragment_shader_vars = R"(
in vec3 normal;
in vec3 fpos;

struct MaterialParameters
{
    vec3 ambient;     // Acm
    vec3 diffuse;     // Dcm
    vec3 specular;    // Scm
    float shininess;  // Srm
    vec3 emission;
    float opacity ;
};

uniform MaterialParameters g_material;
uniform vec3 eyePos ;
out vec4 FragColor;
)";
