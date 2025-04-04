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

vec2 rotate(vec2 v, float a){
    mat2 m = mat2(cos(a), sin(a), -sin(a), cos(a));
    return m*v;
}

void main()
{
    vec4 eyes = texture(shaderTextureEyes, shaderTexCoord);

    vec2 rotated_coords = rotate(shaderTexCoord, 45*3.14159/180);
    vec4 rainbow = texture(shaderRainbow, vec2(rotated_coords.x + time*2, rotated_coords.y + time)) ;
    vec4 maintex = vec4(shaderColor, 1.0f);


    fragmentColor = mix(maintex, rainbow, 0.6)*eyes;
    // fragmentColor = maintex * rainbow;
    // 1.0f is alpha here cause color, we need to specify what alpha means, so turning it rn to something else is useless
}
