#define __TYPE_VERTEX_SHADER__
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_Transform;

out vec3 v_Position;
out vec2 v_TexCoord;

void main() {
    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
    v_Position = (u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0)).xyz;
    v_TexCoord = a_TexCoord;
}

#define __TYPE_FRAGMENT_SHADER__
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec3 v_Position;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main() {
//    o_Color = vec4(v_TexCoord, 0.0, 1.0);
    o_Color = texture(u_Texture, v_TexCoord);
}