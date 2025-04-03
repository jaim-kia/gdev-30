/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
in vec2 shaderTexCoord;
uniform sampler2D shaderTextureEyes;

out vec4 fragmentColor;

void main()
{
    vec4 eyes = texture(shaderTextureEyes, shaderTexCoord);

    fragmentColor = vec4(shaderColor, 1.0f) * eyes;
    // 1.0f is alpha here cause color, we need to specify what alpha means, so turning it rn to something else is useless
}
