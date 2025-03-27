/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
out vec4 fragmentColor;

void main()
{
    fragmentColor = vec4(shaderColor, 1.0f);
    // 1.0f is alpha here cause color, we need to specify what alpha means, so turning it rn to something else is useless
}
