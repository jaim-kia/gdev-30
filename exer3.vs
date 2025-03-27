/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;

uniform mat4 matrix;
out vec3 shaderColor;

void main()
{
    gl_Position = matrix * vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f);
    // 1.0f - homogenous coordinate
    shaderColor = vertexColor;
}
