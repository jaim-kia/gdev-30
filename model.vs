/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;
uniform mat4 normalMatrix;

out vec3 worldSpacePosition;
out vec3 worldSpaceNormal;
out vec3 objectColor;
out vec2 shaderTexCoord;


void main()
{
    worldSpacePosition = (modelMatrix * vec4(vertexPosition, 1.0f)).xyz;
    worldSpaceNormal = (normalMatrix * vec4(vertexNormal, 1.0f)).xyz;
    objectColor = vertexColor;

    gl_Position = projectionViewMatrix * vec4(worldSpacePosition, 1.0f);
    shaderTexCoord = vertexTexCoord;
}
