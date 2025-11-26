#version 460 core

// Create a gradient effect

out vec4 FragColor;

void main() 
{
    FragColor = vec4(
        gl_FragCoord.x / 800.0,
        gl_FragCoord.y / 600.0,
        0.2,
        1.0
    );
}
