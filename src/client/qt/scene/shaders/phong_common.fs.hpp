
static const char *phong_fragment_shader_common = R"(
in vec3 normal;
in vec3 position;

const int MAX_LIGHTS = 10;
const int AMBIENT_LIGHT = 0 ;
const int DIRECTIONAL_LIGHT = 1 ;
const int SPOT_LIGHT = 2 ;
const int POINT_LIGHT = 3 ;

struct LightSourceParameters
{
    int light_type ;
    vec3 color;
    vec4 position;
    vec3 direction;
    float spot_exponent;
    float spot_cos_cutoff;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
};

uniform LightSourceParameters g_light_source[MAX_LIGHTS];

struct MaterialParameters
{
    vec4 ambient;     // Acm
    vec4 diffuse;     // Dcm
    vec4 specular;    // Scm
    float shininess;  // Srm
};

uniform MaterialParameters g_material;

out vec4 FragColor;

vec4 phongIllumination(vec4 dc) {
    vec3 N = normalize(normal);
    vec4 finalColor = vec4(0, 0, 0, 1.0);

    float shadow  = 0.0 ;

    for (int i=0;i<MAX_LIGHTS;i++)
    {
        vec3 L ;
        float att = 1.0 ;

        if ( g_light_source[i].light_type == -1 ) continue ;
        else if ( g_light_source[i].light_type == AMBIENT_LIGHT ) {
            finalColor += vec4(g_light_source[i].color, 1.0) * g_material.ambient ;
            continue ;
        }
        else if ( g_light_source[i].light_type == DIRECTIONAL_LIGHT ) {
            L = normalize(g_light_source[i].direction) ;
        }
        else if ( g_light_source[i].light_type == SPOT_LIGHT ) {
            float dist = length(g_light_source[i].position.xyz - position) ;
            L = normalize(g_light_source[i].position.xyz - position);

            float spotEffect = dot(normalize(g_light_source[i].direction), normalize(-L));
            if (spotEffect > g_light_source[i].spot_cos_cutoff) {
                spotEffect = pow(spotEffect, g_light_source[i].spot_exponent);
                att = spotEffect / (g_light_source[i].constant_attenuation +
                g_light_source[i].linear_attenuation * dist +
                g_light_source[i].quadratic_attenuation * dist * dist);
            }
            else att = 0.0 ;
        }
        else if ( g_light_source[i].light_type == POINT_LIGHT ) {
            float dist = length(g_light_source[i].position.xyz - position);
            L = normalize(g_light_source[i].position.xyz - position);

            att = 1.0 / (g_light_source[i].constant_attenuation +
            g_light_source[i].linear_attenuation * dist +
            g_light_source[i].quadratic_attenuation * dist * dist);
        }

        vec3 E = normalize(-position); // we are in Eye Coordinates, so EyePos is (0,0,0)
        vec3 R = normalize(-reflect(L,N));

        //calculate Diffuse Term:

        vec4 Idiff = vec4(g_light_source[i].color, 1.0) * dc * max(dot(N,L), 0.0f);
        Idiff = clamp(Idiff, 0.0, 1.0);

        // calculate Specular Term:
        vec4 Ispec = vec4(g_light_source[i].color, 1.0) * g_material.specular
        * pow(max(dot(R,E),0.0f),g_material.shininess);
        Ispec = clamp(Ispec, 0.0, 1.0);

        finalColor +=  att*clamp((1 - shadow)*(Ispec + Idiff), 0.0, 1.0);
    }

    return  finalColor ;
}
)";
