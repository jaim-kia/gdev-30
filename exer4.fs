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
uniform float specularity;
uniform vec3 cameraPos;

out vec4 fragmentColor;

vec2 rotate(vec2 v, float a){
    mat2 m = mat2(cos(a), sin(a), -sin(a), cos(a));
    return m*v;
}

void main()
{
    // light position
    vec2 lightPositionTwo = rotate(vec2(5.0f, 0.0f), time);

    vec3 lightPosition = vec3(lightPositionTwo.x, lightPositionTwo.x, lightPositionTwo.y);
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);
    
    // light color
    vec3 lightColor = mix(vec3(1.0f, 1.0f, 1.0f), vec3(0.89f, 0.17f, 0.03f), (sin(2*time)+1)/2);

    // vec3 worldSpaceNormal = normalize((normalMatrix * vec4(worldSpaceNormal, 1.0f)).xyz);
    vec3 worldSpaceNormal = normalize(worldSpaceNormal);

    vec4 eyes = texture(shaderTextureEyes, shaderTexCoord);

    vec2 rotated_coords = rotate(shaderTexCoord, 45*3.14159/180);
    vec4 rainbow = texture(shaderRainbow, vec2(rotated_coords.x + time*2, rotated_coords.y + time));

    // spec
    vec3 refVector = reflect(-lightVector, worldSpaceNormal);
    vec3 specular = pow(max(dot(refVector, normalize(cameraPos - worldSpacePosition)), 0), specularity) * lightColor;

    // diff
    vec3 diffuseColor = objectColor * clamp(dot(lightVector, worldSpaceNormal), 0, 1) * lightColor;
    
    vec3 ambientColor = objectColor * vec3(0.63f, 0.43f, 0.05f);
    // vec3 ambientColor = vec3(0.63f, 0.43f, 0.05f);

    // vec3 finalColor = diffuseColor;
    vec3 finalColor = diffuseColor + specular + ambientColor;
    // vec3 finalColor = specular;
    // vec3 finalColor = dot(refVector, cameraPos - worldSpacePosition) * vec3(1.0, 1.0, 1.0);
    vec4 maintex = vec4(finalColor, 1.0f);

    // fragmentColor = mix(maintex, rainbow, abs(sin(time*1.5)))*eyes;
    fragmentColor = maintex;
}
