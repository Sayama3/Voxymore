#define __TYPE_VERTEX_SHADER__
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

layout(std140, binding = 0) Camera
{
    mat4 ViewProjectionMatrix;
} u_Camera;

layout(push_constant) uniform Transform
{
    mat4 Transform;
} u_RendererUniforms;

struct VertexOutput
{
    vec2 TexCoord;
};


layout (location = 0) out VertexOutput Output;

void main() {
    gl_Position = u_Camera.ViewProjectionMatrix * u_RendererUniforms.Transform * vec4(a_Position, 1.0);
    v_Position = (u_Camera.ViewProjectionMatrix * u_RendererUniforms.Transform * vec4(a_Position, 1.0)).xyz;
    Output.TexCoord = a_TexCoord;
}

#define __TYPE_FRAGMENT_SHADER__
#version 450 core

layout(location = 0) out vec4 o_Color;

layout (location = 0) in vec2 v_TexCoord;

layout (binding = 0) uniform sampler2D u_Texture;

void main() {
    //    o_Color = vec4(v_TexCoord, 0.0, 1.0);
    o_Color = texture(u_Texture, v_TexCoord);
}