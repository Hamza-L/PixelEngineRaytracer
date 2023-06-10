#version 450 //use glsl 4.5

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texUV;

layout(set = 0, binding = 0) uniform UboVP
{
    mat4 V;
    mat4 P;
    vec4 lightPos;
} uboVP;

//unused (replaced by push constants)
layout(set = 0, binding = 1) uniform DynamicUBObj
{
    mat4 M;
    mat4 MinvT;
    int texIndex;
} dynamicUBObj;

layout(push_constant) uniform PObj
{
    mat4 M;
    mat4 MinvT;
} pushObj;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalForFP;
layout(location = 2) out vec3 lightPos;
layout(location = 3) out vec3 positionForFP;
layout(location = 4) out vec2 fragTex;
layout(location = 5) out flat int texID;

void main()
{
    gl_Position = uboVP.P * uboVP.V * pushObj.M * position;
    fragColor = color;

    vec4 tempLPos = uboVP.V * uboVP.lightPos;
    lightPos = tempLPos.xyz;
    vec4 tempPos = uboVP.V * pushObj.M * position;
    positionForFP = tempPos.xyz;
    vec4 tempNorm = uboVP.V * pushObj.MinvT * vec4(normal.xyz, 0.0f);
    normalForFP = vec4(normalize(tempNorm.xyz),0.0f);

    fragTex = texUV;
    texID = dynamicUBObj.texIndex;
}