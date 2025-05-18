#version 330 core
in vec3 Color;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
    vec4 texColor = texture(particleTexture, TexCoord);
    FragColor = vec4(Color, 1.0) * texColor;
    // FragColor = vec4(1.0) * texColor;
    if (FragColor.a < 0.1) discard; // For transparency
}