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
uniform vec3 lightColor;
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

    vec3 lightPosition = vec3(lightPositionTwo.x, 2, lightPositionTwo.y);
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);
    
    // renormalizing to avoid interpolation
    vec3 finalNormal = normalize(worldSpaceNormal);

    // getting the color of the texture
    vec3 eyes = texture(shaderTextureEyes, shaderTexCoord).xyz;
    vec2 rotated_coords = rotate(shaderTexCoord, 45*3.14159/180);
    vec3 rainbow = texture(shaderRainbow, vec2(rotated_coords.x + time*2, rotated_coords.y + time)).xyz;

    vec3 objectColorFinal = mix(objectColor, rainbow, (sin(time)+1)/2)*eyes;

    // specularity
    vec3 refVector = reflect(-lightVector, finalNormal);
    vec3 specular = pow(max(dot(refVector, normalize(cameraPos - worldSpacePosition)), 0), specularity) * lightColor;

    // diffuse
    vec3 diffuseColor = objectColorFinal * max(dot(lightVector, finalNormal), 0) * lightColor;
    
    // ambient
    vec3 ambientColor = objectColorFinal * vec3(0.3f, 0.3f, 0.3f);

    // output color
    vec3 finalColor = diffuseColor + specular + ambientColor;
    vec4 maintex = vec4(finalColor, 1.0f);
    fragmentColor = maintex;
}
