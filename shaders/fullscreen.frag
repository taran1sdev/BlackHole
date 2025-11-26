#version 460 core

out vec4 FragColor;

uniform vec2 resolution;

uniform vec3 camPos;
uniform vec3 camForward;
uniform vec3 camRight;
uniform vec3 camUp;

float sphereSDF(vec3 p, float r) {
    return length(p) - r;
}


// Function to detect collisions
bool raySphere(vec3 rayOrigin, vec3 rayDirection, float radius, out float t) {
    // b = dot product of vectors rayOrigin and rayDirection
    float b = dot(rayOrigin, rayDirection);
    // c = vector rayOrigin^2 - radius^2
    float c = dot(rayOrigin, rayOrigin) - radius*radius;
    // h = quadratic discriminant
    float h = b*b - c;
    // if h < 0 then no solutions or no hit
    if (h < 0.0) return false;
    
    // calculate the distance to the hit
    t = -b - sqrt(h);
    // if the hit is in front of the camera return true
    return t > 0.0;
}

void main() 
{
    // pixel coordinates (0 to 1)
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    
    // trace rays from camera
    vec3 rayOrigin = camPos; 
    vec3 rayDirection = normalize(
            camForward + uv.x * camRight + uv.y * camUp 
    );
    
    float t;
    if (raySphere(rayOrigin, rayDirection, 1.0, t)) {
        // If we have a hit calculate and normatize the collision point
        vec3 hit = rayOrigin + rayDirection * t;
        vec3 normHit = normalize(hit);
        
        // Calculate how much light hits the collision point
        float diff = max(dot(normHit, normalize(vec3(1.0, 1.0, -1.0))), 0.0);

        FragColor = vec4(vec3(diff), 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
