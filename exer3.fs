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
uniform sampler2D shaderRainbow;
uniform float time;

out vec4 fragmentColor;

void main()
{
    vec4 eyes = texture(shaderTextureEyes, shaderTexCoord);
    vec4 rainbow = texture(shaderRainbow, vec2(shaderTexCoord.x + time*2, shaderTexCoord.y + time*2));
    vec4 maintex = vec4(shaderColor, 1.0f) * eyes;


    fragmentColor = mix(maintex, rainbow, 0.6);
    // 1.0f is alpha here cause color, we need to specify what alpha means, so turning it rn to something else is useless
}
