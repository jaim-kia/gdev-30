/******************************************************************************
 * This is a vertex shader that assumes that each vertex has a position and
 * color. The color of the vertex is further manipulated via a uniform, and
 * this color is passed to the fragment shader as an output.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
uniform float glow;
uniform float time;
uniform vec3 color;
vec3 origin = vec3(1.0f, 0.0f, 0.0f);
out vec3 shaderColor;

void main()
{
    // original
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.0f);

    // item 1
    // gl_Position = vec4(vertexPosition.x + 0.25f, vertexPosition.y + 0.25f, vertexPosition.z, 1.0f);
    
    // item 2
    // gl_Position = vec4(vertexPosition.x * abs(sin(time)), vertexPosition.y * abs(sin(time)), vertexPosition.z, 1.0f);
    
    // item 3
    // gl_Position = vec4((origin.x * cos(time) + origin.y * sin(time) + vertexPosition.x),
    //                 (-origin.x * sin(time) + origin.y * cos(time) + vertexPosition.y), vertexPosition.z, 1.0f);
    
    // item 4
    shaderColor = vec3(vertexColor.r, vertexColor.g, vertexColor.b) * glow;
}
