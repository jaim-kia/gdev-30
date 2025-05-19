#version 330 core

in vec2 TexCoord;
in vec3 worldSpacePosition;
in vec3 worldSpaceNormal;
in vec3 objectColor;

out vec4 FragColor;

uniform sampler2D particleTexture;
uniform float time;
uniform vec3 lightColor;
uniform float opacity;

vec2 rotate(vec2 v, float a){
    mat2 m = mat2(cos(a), sin(a), -sin(a), cos(a));
    return m*v;
}

void main()
{
    // light position
    vec2 lightPositionTwo = rotate(vec2(5.0f, 0.0f), time);
    // vec2 lightPositionTwo = vec2(0.0f, 0.0f);
    vec3 lightPosition = vec3(lightPositionTwo.x, 2, lightPositionTwo.y);
    vec3 lightVector = normalize(lightPosition - worldSpacePosition);

    // renormalizing to avoid interpolation
    vec3 finalNormal = normalize(worldSpaceNormal);

    vec3 objectColorFinal = texture(particleTexture, TexCoord).xyz * objectColor;

    // diffuse
    vec3 diffuseColor = objectColorFinal * max(dot(lightVector, finalNormal), 0) * lightColor;

    // ambient
    vec3 ambientColor = objectColorFinal * vec3(0.7f, 0.7f, 0.7f);

    // finalColor
    vec3 finalColor = diffuseColor + ambientColor;
    vec4 maintex = vec4(finalColor, opacity);

    FragColor = maintex;
    if (FragColor.a < 0.1) discard;
}