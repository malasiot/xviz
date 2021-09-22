
static const char *vertex_shader_code = R"(
#version 330

const int MAX_LIGHTS = 10;

layout (location = 0) in vec3 vposition;
out vec3 position;

#ifdef HAS_NORMALS
layout (location = 1) in vec3 vnormal;
out vec3 normal;
#endif

#ifdef HAS_COLORS
layout (location = 2) in vec3 vcolor;
out vec3 color ;
#endif

#ifdef USE_SKINNING
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4  boneWeights;

const int MAX_BONES = 200;
uniform mat4 g_bones[MAX_BONES];
#endif

#ifdef  HAS_UVs
layout (location = 5) in vec2 vuv;
out vec2 uv;
#endif

#ifdef HAS_SHADOWS
   uniform mat4 lsmat ;
   out vec4 lspos ;
#endif

uniform mat4 model ;
uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 mvn ;

void main()
{
#ifdef USE_SKINNING
    mat4 BoneTransform = g_bones[boneIDs[0]] * boneWeights[0];
    BoneTransform     += g_bones[boneIDs[1]] * boneWeights[1];
    BoneTransform     += g_bones[boneIDs[2]] * boneWeights[2];
    BoneTransform     += g_bones[boneIDs[3]] * boneWeights[3];

    vec4 posl    = BoneTransform * vec4(vposition, 1.0);

#ifdef HAS_NORMALS
    vec3 normall = mat3(BoneTransform) * vnormal;
#endif
    gl_PointSize = 4.0;
#else
    vec4 posl = vec4(vposition, 1.0);

#ifdef HAS_NORMALS
    vec3 normall = vnormal;
#endif

#endif // SKINING

    gl_Position  = mvp * posl;

#ifdef HAS_NORMALS
    normal = mvn * normall;
#endif

#ifdef HAS_COLORS
    color = vcolor ;
#endif

#ifdef HAS_UVs
    uv = vuv ;
#endif
    position    = (mv * posl).xyz;
    vec3 fpos = vec3(model * posl);
#ifdef HAS_SHADOWS
    lspos = lsmat * vec4(fpos, 1);
#endif
}
)";
