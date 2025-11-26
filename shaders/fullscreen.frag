#version 460 core

out vec4 FragColour;

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

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453);
}

bool rayPlane(vec3 ro, vec3 rd, vec3 n, float h, out float t)
{
    float denom = dot(rd, n);
    if (abs(denom) < 0.0001) return false;
    t = (h - dot(ro, n)) / denom;
    return t > 0.0;
}

vec3 gridColour(vec3 p)
{
    float scale = 1.0;
    bool line =
        abs(fract(p.x * scale) - 0.5) < 0.01 ||
        abs(fract(p.z * scale) - 0.5) < 0.01;

    return line ? vec3(0.3) : vec3(0.0);
}

bool hitSphere(vec3 pos, Sphere s)
{
    return length(pos - s.center) < s.radius;
}

const float DISK_INNER = 2.0; 
const float DISK_OUTER = 9.0;
const float DISK_HALF_H = 0.15;

bool hitDisk(vec3 pos, vec3 bhCenter)
{
    vec3 p = pos - bhCenter;

    if (abs(p.y) > DISK_HALF_H) return false;

    float r = length(p.xz);
    return (r > DISK_INNER && r < DISK_OUTER);
}

vec3 shadeDisk(vec3 pos, vec3 bhCenter, vec3 rayDir)
{
    vec3 p = pos - bhCenter;
    float r = length(p.xz);
    float h = abs(p.y);

    float tRad = smoothstep(DISK_INNER, DISK_OUTER * 0.6, r);

    // base colour gradient
    vec3 inner = vec3(3.0, 2.3, 1.0);
    vec3 outer = vec3(1.3, 0.5, 0.1);
    vec3 col   = mix(inner, outer, tRad);

    // vertical brightness
    float tH = smoothstep(0.0, DISK_HALF_H, h);
    col *= 1.0 + tH * 2.0;

    // turbulence / clumpiness
    float n1 = hash(p.xz * 6.0);
    float n2 = hash(p.xz * 0.9 + 10.0);
    float turb = mix(n1, n2, 0.5);
    col *= mix(0.7, 1.6, turb);

    // approximate Doppler beaming
    vec3 tangent = normalize(vec3(-p.z, 0.0, p.x)); // orbital direction
    float dop = dot(-rayDir, tangent);
    float boost = 1.0 + 1.8 * max(dop, 0.0);
    float dim   = 1.0 - 0.8 * max(-dop, 0.0);
    col *= mix(dim, boost, 0.9);

    return col;
}

vec3 traceGeodesic(vec3 ro, vec3 rd, Sphere blackHole, Sphere star, Sphere planet, out bool hitSomething)
{
    hitSomething = false;
    vec3 colour = vec3(0.0);

    vec3 bhCenter = blackHole.center;
    float eventR  = blackHole.radius; 

    const int   STEPS     = 480;
    const float BASE_STEP = 0.18;
    const float G_STRENGTH = 0.8;
    const float MAX_R     = 80.0;

    vec3 pos = ro;
    vec3 dir = normalize(rd);

    for (int i = 0; i < STEPS; i++)
    {
        vec3 rel = pos - bhCenter;
        float r  = length(rel);

        // fell into the black hole
        if (r < eventR)
        {
            colour = vec3(0.0);
            hitSomething = true;
            break;
        }

        // star / planet hits
        if (hitSphere(pos, star))
        {
            vec3 n = normalize(pos - star.center);
            float diff = max(dot(n, normalize(vec3(1,1,-1))), 0.0);
            colour = star.colour * diff;
            hitSomething = true;
            break;
        }

        if (hitSphere(pos, planet))
        {
            vec3 n = normalize(pos - planet.center);
            float diff = max(dot(n, normalize(vec3(1,1,-1))), 0.0);
            colour = planet.colour * diff;
            hitSomething = true;
            break;
        }

        // accretion disk
        if (hitDisk(pos, bhCenter))
        {
            colour = shadeDisk(pos, bhCenter, dir);
            hitSomething = true;
            break;
        }

        if (r > MAX_R)
        {
            float v = clamp((r - MAX_R) * 0.02, 0.0, 1.0);
            colour = mix(vec3(0.02, 0.0, 0.03), vec3(0.08, 0.01, 0.03), v);
            break;
        }

        vec3 n = normalize(rel);

        vec3 tangential = dir - n * dot(dir, n);

        float invr2 = 1.0 / max(r * r, 0.001);
        dir += -G_STRENGTH * invr2 * tangential;
        dir = normalize(dir);

        float step = BASE_STEP * mix(0.4, 2.0, clamp((r - eventR) / 10.0, 0.0, 1.0));
        pos += dir * step;
    }

    return colour;
}

void main()
{
    // Screen coordinates
    vec2 uv = (gl_FragCoord.xy / resolution) * 2.0 - 1.0;
    uv.x *= resolution.x / resolution.y;

    vec3 ro = camPos;
    vec3 rd = normalize(camForward + uv.x * camRight + uv.y * camUp);

    Sphere blackHole = Sphere(vec3(0.0, 2.5, 0.0), 1.0, vec3(0.0));
    Sphere star      = Sphere(vec3(16.0, 5.0,-13.0), 6.0, vec3(1.0,0.9,0.2));
    Sphere planet    = Sphere(vec3(12.0, 4.0, 15.0), 4.6, vec3(0.2,0.4,1.0));

    bool hitSomething = false;
    vec3 colour = traceGeodesic(ro, rd, blackHole, star, planet, hitSomething);

    if (!hitSomething)
    {
        float tFloor;
        if (rayPlane(ro, rd, vec3(0,1,0), -1.0, tFloor))
        {
            vec3 p = ro + rd * tFloor;
            colour = gridColour(p) * 0.7;
        }
        else
        {
            // empty space
            colour = vec3(0.01, 0.0, 0.02);
        }
    }

    //Event horizon and photon ring
    vec3 bhCenter = blackHole.center;

    vec3 toBH = normalize(bhCenter - ro);
    float facing = dot(rd, toBH);

    if (facing > 0.0)
    {
        float eventR  = blackHole.radius;
        float photonR = 1.7;
        float ringW   = 0.06;

        float b = length(cross(ro - bhCenter, rd));

        float horizonMask =
            smoothstep(eventR + 0.02, eventR - 0.02, b);
        colour = mix(colour, vec3(0.0), horizonMask);

        float ringMask =
            smoothstep(photonR + ringW, photonR, b) *
            smoothstep(photonR - ringW, photonR, b);

        colour += vec3(1.2, 0.4, 0.1) * ringMask * 2.2;
    }

    FragColour = vec4(colour, 1.0);
}

