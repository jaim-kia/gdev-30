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
layout (location = 3) in vec2 normals;

uniform mat4 matrix;
out vec3 shaderColor;
out vec2 shaderTexCoord;


void main()
{
    gl_Position = matrix * vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f);
    // 1.0f - homogenous coordinate
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
}
