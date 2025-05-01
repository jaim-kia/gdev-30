/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec2 shaderTexCoord;
in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;
in vec3 objectColor;

uniform sampler2D shaderTextureEyes;
uniform sampler2D shaderRainbow;
uniform mat4 normalMatrix;
uniform float time;

out vec4 fragmentColor;

vec2 rotate(vec2 v, float a){
    mat2 m = mat2(cos(a), sin(a), -sin(a), cos(a));
    return m*v;
}

void main()
{
    vec3 lightPosition = vec3(0.0f, 1.0f, 0.0f);
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);
    vec3 worldSpaceNormal = normalize((normalMatrix * vec4(worldSpaceNormal, 1.0f)).xyz);

    vec4 eyes = texture(shaderTextureEyes, shaderTexCoord);

    vec2 rotated_coords = rotate(shaderTexCoord, 45*3.14159/180);
    vec4 rainbow = texture(shaderRainbow, vec2(rotated_coords.x + time*2, rotated_coords.y + time));

    vec3 diffuseColor = objectColor * clamp(dot(lightVector, worldSpaceNormal), 0, 1) * vec3(1.0, 1.0, 1.0);
    vec4 maintex = vec4(diffuseColor, 1.0f);

    // fragmentColor = mix(maintex, rainbow, abs(sin(time*1.5)))*eyes;
    fragmentColor = maintex;
}
