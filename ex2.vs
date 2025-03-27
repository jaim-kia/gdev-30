/******************************************************************************
 * This is a really simple vertex shader that simply sets the output vertex's
 * position to be the same as the input.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition; // we are adding a new attribute
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 vertexTexCoord;
layout (location = 3) in vec2 vertexAnim;
uniform float t;
uniform float d;
float adjust = 0.1f; // adjust displacement
out vec3 shaderColor;
out vec2 shaderTexCoord;
out vec2 shaderAnim;

void main()
{
    
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f); // 4th attribute is a homogenous coordinate (the w) 

    if (vertexAnim.x == 2) {
        gl_Position = mix(gl_Position, vec4(vertexPosition.x + (-1 * adjust), vertexPosition.y + (-3 * adjust), vertexPosition.z, 1), clamp(d, 0, 1));
    }
    if (vertexAnim.x == 3) {
        gl_Position = mix(gl_Position, vec4(vertexPosition.x + (-4 * adjust), vertexPosition.y + (-3 * adjust), vertexPosition.z, 1), clamp(d, 0, 1));
    }
    if (vertexAnim.x == 4) {
        gl_Position = mix(gl_Position, vec4(vertexPosition.x + (-3 * adjust), vertexPosition.y + (3 * adjust), vertexPosition.z, 1), clamp(d, 0, 1));
    }
    if (vertexAnim.x == 5) {
        gl_Position = mix(gl_Position, vec4(vertexPosition.x + (0.5 * adjust), vertexPosition.y + (3 * adjust), vertexPosition.z, 1), clamp(d, 0, 1));
    }
    if (vertexAnim.x == 6) {
        gl_Position = mix(gl_Position, vec4(vertexPosition.x + (3 * adjust), vertexPosition.y + (-3 * adjust), vertexPosition.z, 1), clamp(d, 0, 1));
    }

    shaderAnim = vertexAnim;
    shaderColor = vertexColor;
    shaderTexCoord = vertexTexCoord;
}
