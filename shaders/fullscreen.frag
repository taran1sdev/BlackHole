#version 460


in vec2 TexCoord;
out vec4 FragColour;

uniform sampler2D screenTexture;

void main()
{
    FragColour = texture(screenTexture, TexCoord);
}
