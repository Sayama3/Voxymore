#define __TYPE_VERTEX_SHADER__
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;


layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjectionMatrix;
};
layout(std140, binding = 1) uniform Model
{
    mat4 u_Transform;
    int u_EntityId;
};

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec4 v_Color;
layout(location = 2) out flat int v_EntityId;

void main()
{
    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
    v_Position = (u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0)).xyz;
    v_Color = a_Color;
    v_EntityId = u_EntityId;
}

#define __TYPE_FRAGMENT_SHADER__
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_Entity; // -1 = no entity.

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec4 v_Color;
layout(location = 2) in flat int v_EntityId;

void main() {
    o_Color = v_Color;
    o_Entity = v_EntityId;
}
