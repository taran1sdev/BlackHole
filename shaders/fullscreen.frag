#version 460 core

in vec2 vUV;
out vec4 FragColour;

layout(binding = 0) uniform sampler2D renderedImage;

uniform float uTime;

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 345.45));
    p += dot(p, p + 35.345);
    return fract(p.x * p.y);
}

// generates starry background
vec3 starrySky(vec2 uv) {    
    // no of stars
    vec2 p = uv * 600.0;

    vec2 cell = floor(p);
    vec2 f = fract(p);
    
    float rand1 = hash(cell);
    float rand2 = hash(cell * 1.23 + 4.98);
        
    // make distribution a bit more random

    vec2 center = vec2(rand1, rand2);
    float d = length(f - center);

    // density of stars per cell
    float star = step(0.995, hash(cell + 44.7)); 

    float glow = exp(-12.0 * d * d); 
   
    // twinkly stars
    float twinkle = 0.85 + 0.15 * sin(uTime * 1.7 + hash(cell) * 25.0);

    vec3 starColour = vec3(1.0, 0.95, 0.9);

    return star * glow * twinkle * starColour;
}

void main()
{
    vec3 bg = starrySky(vUV);

    vec3 bh = texture(renderedImage, vUV).rgb;

    float I = length(bh);

    float mask = smoothstep(0.02, 0.08, I);
    
    vec3 colour = mix(bg, bh, mask);

    FragColour = vec4(colour, 1.0);
}
