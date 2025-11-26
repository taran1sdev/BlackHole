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
bool raySphere(vec3 rayOrigin, vec3 rayDirection, vec3 center, float radius, out float t) {
    // get the difference between the ray origin and center of the sphere
    vec3 originCenter = rayOrigin - center;
    // b = dot product of vectors rayOrigin and rayDirection
    float b = dot(originCenter, rayDirection);
    // c = vector rayOrigin^2 - radius^2
    float c = dot(originCenter, originCenter) - radius*radius;
    // h = quadratic discriminant
    float h = b*b - c;
    // if h < 0 then no solutions or no hit
    if (h < 0.0) return false;
    
    // calculate the distance to the hit
    t = -b - sqrt(h);
    // if the hit is in front of the camera return true
    return t > 0.0;
}

// Functions to create a grid effect on the floor

// detect collisions with our floor
bool rayPlane(vec3 rayOrigin, vec3 rayDirection, vec3 planeNormal, float planeY, out float t) 
{
    float denom = dot(rayDirection, planeNormal);
    // Return false if ray is parallel to plane
    if (abs(denom) < 0.0001) return false;
   
    // calculate distance 
    t = (planeY - dot(rayOrigin, planeNormal)) / denom;
    return t > 0.0;
}

// Decide to return lighter grid line or black background
vec3 gridColour(vec3 pos)
{
    float scale = 1.0;

    bool grid = abs(fract(pos.x * scale) - 0.5) < 0.01 ||
                 abs(fract(pos.z * scale) - 0.5) < 0.01;
    
    // light lines
    if (grid) return vec3(0.3);
    
    // Darker ground
    return vec3(0.0);
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
    
    vec3 blackHolePos = vec3(0.0, 1.5, 0.0);
    float blackHoleRadius = 1.0;

    float tSphere;
    bool hitSphere = raySphere(rayOrigin, rayDirection, blackHolePos, blackHoleRadius, tSphere);

    float tPlane;
    bool hitPlane = rayPlane(rayOrigin, rayDirection, vec3(1.0, 1.0, 1.0).yxx, -1.0, tPlane);

    float t = 1e9;
    vec3 colour = vec3(0.0);
    
    if (hitSphere && tSphere < t) {
        t = tSphere;
        vec3 hitPoint = rayOrigin + rayDirection * tSphere;
        vec3 normal = normalize(hitPoint - blackHolePos);
        float diff = max(dot(normal, normalize(vec3(1,1,-1))), 0.0);
        colour = vec3(diff, 0.0, 0.0);
    }

    if (hitPlane && tPlane < t) {
        t = tPlane;
        vec3 hitPoint = rayOrigin + rayDirection * tPlane;
        colour = gridColour(hitPoint);
    }

    FragColor = vec4(colour, 1.0);
}
