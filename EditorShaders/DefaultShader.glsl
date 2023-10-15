#define __TYPE_VERTEX_SHADER__
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 2) in vec3 a_Normal;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 3) in vec4 a_Color;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjectionMatrix;
};
layout(std140, binding = 1) uniform Model
{
    mat4 u_Transform;
    int u_EntityId;
};

layout(std140, binding = 2) uniform Lights
{
    vec3 lightDir = {0, -1, 0};
};

struct TextureInfo
{
    int Index;
    int TexCoord;
};

struct NormalTextureInfo
{
    int Index;
    int TexCoord;
    float Scale;
};

struct OcclusionTextureInfo
{
    int Index;
    int TexCoord;
    float Strenght;
};

struct MetallicRoughtness
{
    vec4 BaseColorFactor;
    bool HasBaseColorTexture;
    TextureInfo BaseColorTexture;
    float MetallicFactor;
    float RoughtnessFactor;
    bool HasMetallicRoughnessTexture;
    TextureInfo MetallicRoughnessTexture;
};

layout(std140, binding = 3) uniform MaterialParameters
{
    bool HasRoughness = false;
    MetallicRoughtness PbrMetallicRoughness;
    bool HasNormalTexture = false;
    NormalTextureInfo NormalTexture;
    bool HasOcclusionTexture = false;
    OcclusionTextureInfo OcclusionTexture;
    bool HasEmissiveTexture = false;
    TextureInfo EmissiveTexture;
    vec3 EmissiveFactor = {0,0,0};
    int AlphaMode = 0;
    float AlphaCutoff = 0.5f;
    bool DoubleSided = false;
};

layout (location = 0) out vec2 v_TexCoord;
layout (location = 1) out flat int v_EntityId;
layout (location = 2) out vec4 v_Color;

void main() {
    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
    v_EntityId = u_EntityId;
    v_Color = a_Color;
}

#define __TYPE_FRAGMENT_SHADER__
#version 450 core

layout (location = 0) out vec4 o_Color;
layout (location = 1) out int o_Entity; // -1 = no entity. (for now.)

layout (location = 0) in vec2 v_TexCoord;
layout (location = 1) in flat int v_EntityId;
layout (location = 2) in vec4 v_Color;

//layout (binding = 0) uniform sampler2D u_Texture;

void main() {
    o_Color = v_Color;
    //TODO: Add texture... If any.
    //    o_Color = texture(u_Texture, v_TexCoord);
    //    float entityColor = v_EntityId / 20.0f;
    //    o_Color = vec4(entityColor, entityColor, entityColor, 1.0f);
    o_Entity = v_EntityId;
}