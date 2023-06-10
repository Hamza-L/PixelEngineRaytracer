#version 450 //glsl version

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 normalForFP;
layout(location = 2) in vec3 lightPos;
layout(location = 3) in vec3 positionForFP;
layout(location = 4) in vec2 fragTex;
layout(location = 5) in flat int texID;


layout(set = 1, binding = 0) uniform sampler2D texSampler[16];

layout(location = 0) out vec4 outColor; //final output color, must have location 0. we output to the first attachment

void main()
{
    //vec4 normal = normalForFP + vec4(texture(texSampler[1], fragTex).xy,0.0f,0.0f);
    vec4 normal = normalize(normalForFP);

    vec3 lightDirection = normalize(lightPos - positionForFP );
    vec3 viewDirection = normalize(-positionForFP );
    vec3 halfVector = normalize( lightDirection + viewDirection);

    float diffuse = max(0.0f,dot( normal.xyz, lightDirection));
    float specular = max(0.0f,dot( normal.xyz, halfVector ) );
    float distanceFromLight = length(lightPos - positionForFP);

    if (diffuse == 0.0) {
        specular = 0.0;
    } else {
        specular = pow( specular, 32.0f );
    }

    //here we use the texture image

    vec3 albedo;
    if(texID >= 0 )
    {
        albedo = texture(texSampler[texID], fragTex).xyz;
    } else
    {
        albedo = fragColor.xyz;
    }

    vec3 scatteredLight =  albedo * diffuse;
    vec3 reflectedLight = vec3(1.0f,1.0f,1.0f) * specular;
    vec3 ambientLight = albedo.xyz * 0.08f;

    //outColor = vec4(normalForFP.xyz,1.0f);

    outColor = vec4(min( ambientLight + scatteredLight + reflectedLight, vec3(1,1,1)), fragColor.w);
}