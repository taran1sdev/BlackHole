#version 460 core

in vec2 vUV;
out vec4 FragColour;

layout(binding = 0) uniform sampler2D renderedImage;

void main()
{
    vec4 bh = texture(renderedImage, vUV);
    FragColour = bh;
}
