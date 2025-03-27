/******************************************************************************
 * This is a really simple fragment shader that simply sets the output fragment
 * color to yellow.
 *
 * Happy hacking! - eric
 *****************************************************************************/

#version 330 core

in vec3 shaderColor;
in vec2 shaderTexCoord;
in vec2 shaderAnim;
uniform sampler2D shaderTextureA;
uniform sampler2D shaderTextureB;
uniform sampler2D displaceTexture;
out vec4 fragmentColor;
uniform float t;
uniform int i;
uniform float d;

vec2 transform = vec2(sin(t * 0.5f), sin(t * 0.5f));

vec4 colorA = texture(shaderTextureA, shaderTexCoord); // mizisua young
// vec4 colorB = texture(shaderTextureB, shaderTexCoord); // mizisua adult


void main()
{
    vec4 displacement = texture(displaceTexture, shaderTexCoord + transform); // actual displacement texture
    float value = (displacement.r * 2) - 1; // convert the range of r from [0,1] to [-1, 1]
    vec2 discs = vec2(-0.05f, -0.05f) * value;
    vec4 displacedTex = texture(shaderTextureB, shaderTexCoord + discs); // actual colorB
    vec4 displacedTex_SHIFT = texture(shaderTextureB, (shaderTexCoord + discs) + vec2(cos(t) * -0.01f, 1)); // colorB shifted to the left for chromatic abberation

    // set colors
    vec4 red = vec4(shaderColor.x - 0.18f, shaderColor.y - 0.77f, shaderColor.z - 0.77f, 1.0f);
    vec4 darkerRed = vec4(shaderColor.x - 0.45f, shaderColor.y - 0.93f, shaderColor.z - 0.89f, 1.0f);
    vec4 lighterRed = vec4(shaderColor.x - 0.0f, shaderColor.y - 0.48f, shaderColor.z - 0.43f, 1.0f);
    vec4 yellow = vec4(shaderColor.x - 0.02f, shaderColor.y - 0.04f, shaderColor.z - 0.66f, 1.0f);

    vec4 totalRed = vec4(shaderColor.x - 0.0f, shaderColor.y - 1.0f, shaderColor.z - 1.0f, 1.0f);
    vec4 totalGreen = vec4(shaderColor.x - 1.0f, shaderColor.y - 0.0f, shaderColor.z - 1.0f, 1.0f);
    vec4 totalBlue = vec4(shaderColor.x - 1.0f, shaderColor.y - 1.0f, shaderColor.z - 0.0f, 1.0f);

    vec4 black = vec4(shaderColor.x - 1.0f, shaderColor.y - 1.0f, shaderColor.z - 1.0f, .7f);

    // setup for chromatic abberation
    vec4 displacedTex_RED = displacedTex_SHIFT * totalRed;
    vec4 displacedTex_GREEN = displacedTex * totalGreen;
    vec4 displacedTex_BLUE = displacedTex * totalBlue;

    vec4 screen_DT_R_G = vec4(1, 1, 1, 0) - ((vec4(1, 1, 1, 0) - displacedTex_GREEN) * (vec4(1, 1, 1, 0) - displacedTex_RED));
    vec4 screen_DT_RG_B = vec4(1, 1, 1, 0) - ((vec4(1, 1, 1, 0) - displacedTex_BLUE) * (vec4(1, 1, 1, 0) - screen_DT_R_G));

    if (shaderAnim.x > 0 && d <= 0.1) {
        if (shaderAnim.y == 0)
            fragmentColor = mix(colorA, colorA * yellow, abs(clamp(sin(t), 0, 1)));
        else if (shaderAnim.y == 1)
            fragmentColor = mix(colorA, colorA * red, abs(clamp(sin(t), 0, 1)));
        else if (shaderAnim.y == 2)
            fragmentColor = mix(colorA, colorA * darkerRed, abs(clamp(sin(t), 0, 1)));
        else if (shaderAnim.y == 3)
            fragmentColor = mix(colorA, colorA * red, abs(clamp(sin(t), 0, 1)));
        else if (shaderAnim.y == 4)
            fragmentColor = mix(colorA, colorA * darkerRed, abs(clamp(sin(t), 0, 1)));
        else
            fragmentColor = mix(colorA, colorA * lighterRed, abs(clamp(sin(t), 0, 1)));
    }
    else if (shaderAnim.x > 0 && d > 0.1) { // change to adult mizisua
        if (shaderAnim.y == 0) {
            fragmentColor = mix(screen_DT_RG_B, colorA * yellow, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
            fragmentColor = mix(screen_DT_RG_B, colorA, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
        }
            
        else if (shaderAnim.y == 1) {
            fragmentColor = mix(screen_DT_RG_B, colorA * red, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
            fragmentColor = mix(screen_DT_RG_B, colorA, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
        }
            
        else if (shaderAnim.y == 2) {
            fragmentColor = mix(screen_DT_RG_B, colorA * darkerRed, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
            fragmentColor = mix(screen_DT_RG_B, colorA, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
        }
            
        else if (shaderAnim.y == 3) {
            fragmentColor = mix(screen_DT_RG_B, colorA * red, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
            fragmentColor = mix(screen_DT_RG_B, colorA, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
        }
            
        else if (shaderAnim.y == 4) {
            fragmentColor = mix(screen_DT_RG_B, colorA * darkerRed, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
            fragmentColor = mix(screen_DT_RG_B, colorA, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
        }
            
        else {
            fragmentColor = mix(screen_DT_RG_B, colorA * lighterRed, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
            fragmentColor = mix(screen_DT_RG_B, colorA, abs(clamp(sin(t * 2 - radians(90)), 0, 1)));
        }
            

    }
}
