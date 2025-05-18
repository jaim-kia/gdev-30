#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 Color;
out vec2 TexCoord;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform vec3 acolor;

void main()
{
    Color = acolor;
    TexCoord = aTexCoord;
    gl_Position = projectionViewMatrix * modelMatrix * vec4(aPos, 1.0);
}