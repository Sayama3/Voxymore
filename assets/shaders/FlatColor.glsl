#define __TYPE_FRAGMENT_SHADER__
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec3 v_Position;
in vec4 v_Color;

void main() {
    o_Color = v_Color;
}

#define __TYPE_VERTEX_SHADER__
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_Transform;

out vec3 v_Position;
out vec4 v_Color;

void main() {
    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
    v_Position = (u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0)).xyz;
    v_Color = a_Color;
}