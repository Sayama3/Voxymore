#define __TYPE_VERTEX_SHADER__
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjectionMatrix;
};
layout(std140, binding = 1) uniform Model
{
    mat4 u_Transform;
};
struct VertexOutput
{
    vec2 TexCoord;
};


layout (location = 0) out VertexOutput Output;

void main() {
    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
    Output.TexCoord = a_TexCoord;
}

#define __TYPE_FRAGMENT_SHADER__
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_Entity; // -1 = no entity. (for now.)

layout (location = 0) in vec2 v_TexCoord;

layout (binding = 0) uniform sampler2D u_Texture;

void main() {
    //    o_Color = vec4(v_TexCoord, 0.0, 1.0);
    o_Color = texture(u_Texture, v_TexCoord);
    o_Entity = 51;
}