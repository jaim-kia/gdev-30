#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 vertexNormal;

out vec3 objectColor;
out vec2 TexCoord;
out vec3 worldSpacePosition;
out vec3 worldSpaceNormal;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;
uniform vec3 acolor;

void main()
{
    worldSpacePosition = (modelMatrix * vec4(aPos, 1.0f)).xyz;
    worldSpaceNormal = (normalMatrix * vec4(vertexNormal, 1.0f)).xyz;
    objectColor = acolor;

    gl_Position = projectionViewMatrix * vec4(worldSpacePosition, 1.0);
    TexCoord = aTexCoord;
}