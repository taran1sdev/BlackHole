#version 460 core

out vec4 FragColor;

uniform vec2 resolution;

uniform vec3 camPos;
uniform vec3 camForward;
uniform vec3 camRight;
uniform vec3 camUp;

struct Sphere {
    vec3 center;
    float radius;
    vec3 colour;
};

// Function to detect collisions
bool raySphere(vec3 rayOrigin, vec3 rayDirection, Sphere s, out float t) {
    // get the difference between the ray origin and center of the sphere
    vec3 originCenter = rayOrigin - s.center;
    // b = dot product of vectors rayOrigin and rayDirection
    float b = dot(originCenter, rayDirection);
    // c = vector rayOrigin^2 - radius^2
    float c = dot(originCenter, originCenter) - s.radius * s.radius;
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

// Rotation matrix - Rodrigues' formula
mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float c = cos(angle);
    float s = sin(angle);
    float oc = 1.0 - c;

    return mat3(
        oc*axis.x*axis.x + c, oc*axis.x*axis.y - axis.z*s, oc*axis.x*axis.z + axis.y*s,
        oc*axis.y*axis.x + axis.z*s, oc*axis.y*axis.y + c, oc*axis.y*axis.z - axis.x*s,
        oc*axis.z*axis.x - axis.y*s, oc*axis.z*axis.y + axis.x*s, oc*axis.z*axis.z + c
    );
}

// Gravitational lensing (approximate)
vec3 bendRay(vec3 rayOrigin, vec3 rayDirection, vec3 bCenter)
{
    vec3 ro = rayOrigin - bCenter;
    vec3 rd = normalize(rayDirection);

    // calculate the impact parameter
    float b = length(cross(ro, rd));

    // if it's far away, no bend
    if (b > 10.0) return rd;

    // k determines the strength of the lensing effect
    float k = 0.5;
    // calculate the angle of the bend
    float alpha = k / max(b, 0.001);
    
    // calculate the direction towards the black hole
    vec3 toB = normalize(-ro);
    // calculate the rotational axis
    vec3 axis = normalize(cross(rd, toB));

    // if ray is headed towards the black hole, no bend
    if (length(axis) < 0.0001) return rd;

    // Apply the rotation matrix, normalize and return the new direction
    mat3 R = rotationMatrix(axis, alpha);
    return normalize(R * rd);
}

void main() 
{
    Sphere blackHole;
    blackHole.center = vec3(0.0, 2.5, 0.0);
    blackHole.radius = 1.0;
    blackHole.colour = vec3(1.0, 0.1, 0.1);

    Sphere star;
    star.center = vec3(16.0, 2.0, -13.0);
    star.radius = 1.0;
    star.colour = vec3(1.0, 0.9, 0.2);

    Sphere planet;
    planet.center = vec3(12.0, 2.0, 15);
    planet.radius = 0.6;
    planet.colour = vec3(0.2, 0.4, 1.0);

    // pixel coordinates (0 to 1)
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    
    // trace rays from camera
    vec3 rayOrigin = camPos; 
    vec3 rayDirection = normalize(
            camForward + uv.x * camRight + uv.y * camUp 
    );
   
    // apply lensing effect
    rayDirection = bendRay(rayOrigin, rayDirection, blackHole.center);

    float tPlane;
    bool hitPlane = rayPlane(rayOrigin, rayDirection, vec3(0.0, 1.0, 0.0), -1.0, tPlane);

    float closest = 1e9;
    vec3 colour = vec3(0.0);
    vec3 hitPoint;
    vec3 normal;
    
    float t1;
    if (raySphere(rayOrigin, rayDirection, blackHole, t1) && t1 < closest) {
        closest = t1;
        hitPoint = rayOrigin + rayDirection * t1;
        normal = normalize(hitPoint - blackHole.center);
        float diff = max(dot(normal, normalize(vec3(1,1,-1))), 0.0);
        colour = blackHole.colour * diff;
    }

    float t2;
    if (raySphere(rayOrigin, rayDirection, star, t2) && t2 < closest) {
        closest = t2;
        hitPoint = rayOrigin + rayDirection * t2;
        normal = normalize(hitPoint - star.center);
        float diff = max(dot(normal, normalize(vec3(1,1,-1))), 0.0);
        colour = star.colour * diff;
    }

    float t3;
    if (raySphere(rayOrigin, rayDirection, planet, t3) && t3 < closest) {
        closest = t3;
        hitPoint = rayOrigin + rayDirection * t3;
        normal = normalize(hitPoint - planet.center);
        float diff = max(dot(normal, normalize(vec3(1,1,-1))), 0.0);
        colour = planet.colour * diff;
    }
    
    if (hitPlane && tPlane < closest) {
        closest = tPlane;
        colour = gridColour(rayOrigin + rayDirection * tPlane);
    }

    FragColor = vec4(colour, 1.0);
}
