/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
in vec2 shaderTexCoord;
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
uniform float time;
out vec4 fragmentColor;

void main()
{

    vec2 shaderTexCoordOrig =  vec2(2*(shaderTexCoord.s-0.5) + 1, 2*(shaderTexCoord.t-0.5) + 1);
    vec4 colorA = texture(shaderTextureA, shaderTexCoordOrig);

    // Exercise 1 - 1
    // vec2 shaderTexCoord = vec2(-shaderTexCoord.s, shaderTexCoord.t);

    vec2 shaderTexCoord = vec2(shaderTexCoord.s + sin(time), shaderTexCoord.t);
    vec4 colorB = texture(shaderTextureB, shaderTexCoord);

    // 2. colors

    // 3. beyond  0 1

    // fragmentColor = vec4(shaderColor, 1.0f) * texture(shaderTextureB, shaderTexCoord);
    // 1.0f is alpha here cause color, we need to specify what alpha means, so turning it rn to something else is useless
    
    fragmentColor = vec4(shaderColor, 1.0f) * mix(colorA, colorB, (sin(time) + 1)/2);
    // fragmentColor = vec4(shaderColor, 1.0f) * mix(colorA, colorB, (sin(time)));
    // fragmentColor = vec4(shaderColor, 1.0f) * mix(colorA, colorB, -0.5f);
}
