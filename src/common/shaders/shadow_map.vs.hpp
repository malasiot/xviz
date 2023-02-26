
static const char *shadow_map_shader_vs = R"(
#version 330

  layout (location = 0) in vec3 aPos;

  uniform mat4 lightSpaceMatrix;
  uniform mat4 model;

  void main()
  {
     gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
  }
)" ;

static const char *shadow_debug_shader_vs = R"(
#version 330

  layout (location = 0) in vec3 aPos;
  layout (location = 1) in vec2 aTexCoords;

  out vec2 TexCoords;

  void main()
  {
     TexCoords = aTexCoords;
     gl_Position = vec4(aPos, 1.0);
  }
)" ;
