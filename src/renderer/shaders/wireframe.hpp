static const char *wireframe_shader_gs = R"(
#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
out vec3 vBC;

void main()
{

    vBC = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    vBC = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    vBC = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

}
)";



static const char *wireframe_shader_fs = R"(
#version 330

layout (location = 0) out vec4 outColor;
in vec3 vBC;
uniform vec4 color ;
uniform float width ;
uniform vec4 fill ;

float edgeFactor() {
  vec3 d = fwidth(vBC);
  vec3 f = step(d * width, vBC);
  return min(min(f.x, f.y), f.z);
}

void main(){
    float d = edgeFactor()  ;
    float i = exp2(-2.0*d*d);
    outColor = i*color + (1.0 - i)*fill;
}
)";


