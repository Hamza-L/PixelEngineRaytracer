#version 450 //glsl version

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 normalForFP;
layout(location = 2) in vec3 positionForFP;
layout(location = 3) in vec2 fragTex;
layout(location = 4) in flat int texID;

layout(set = 1, binding = 0) uniform sampler2D texSampler[16];

layout(location = 0) out vec4 outColor; //final output color, must have location 0. we output to the first attachment

void main()
{
    vec3 albedo;
    if(texID >= 0 )
    {
        albedo = texture(texSampler[texID], fragTex).xyz;
    } else
    {
        albedo = fragColor.xyz;
    }

    outColor = vec4(albedo, fragColor.w);
}