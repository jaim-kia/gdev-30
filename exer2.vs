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
uniform float time;

out vec3 shaderColor;
out vec2 shaderTexCoord;

void main()
{
    gl_Position = vec4(vertexPosition.x, vertexPosition.y + 0.5*sin(time), vertexPosition.z, 1.0f);
    gl_Position = vec4(vertexPosition.x, vertexPosition.y , vertexPosition.z, 1.0f);
    // 1.0f - homogenous coordinate
    
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
}
