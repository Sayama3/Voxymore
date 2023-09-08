#define __TYPE_FRAGMENT_SHADER__
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec4 v_Color;

void main() {
    o_Color = v_Color;
}

#define __TYPE_VERTEX_SHADER__
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;


layout(std140, binding = 0) Camera
{
    mat4 u_ViewProjectionMatrix;
};

layout(push_constant) uniform Transform
{
    mat4 Transform;
} u_RendererUniforms;

layout(location = 0)out vec3 v_Position;
layout(location = 1)out vec4 v_Color;

void main() {
    gl_Position = u_Camera.ViewProjectionMatrix * u_RendererUniforms.Transform * vec4(a_Position, 1.0);
    v_Position = (u_Camera.ViewProjectionMatrix * u_RendererUniforms.Transform * vec4(a_Position, 1.0)).xyz;
    v_Color = a_Color;
}