#version 460 core

out vec4 FragColor;

uniform vec2 resolution;

void main() 
{
    // pixel coordinates (0 to 1)
    vec2 uv = gl_FragCoord.xy / resolution;
    
    // convert to -1 to +1 range
    vec2 p = uv * 2.0 - 1.0;
    
    // visualize coordinate
    FragColor = vec4(
        p * 0.5 + 0.5,
        0.0,
        1.0
     );
}
