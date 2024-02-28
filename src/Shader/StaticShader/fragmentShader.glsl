#version 430 core

struct Ray {
    vec3 origin;
    vec3 direction;
};

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iViewMatrix;

uniform sampler3D iVolume;

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
layout (location = 3) out vec4 gDepth;

bool intersectRayAABB(Ray ray, vec3 minBounds, vec3 maxBounds, out vec3 t1, out vec3 t2, out float tMin, out float tMax) {
    vec3 invDirection = 1.0 / ray.direction;
    t1 = (minBounds - ray.origin) * invDirection;
    t2 = (maxBounds - ray.origin) * invDirection;

    // TODO: check if we can take them off since our ray direction is always positive
    vec3 tMinVec = min(t1, t2);
    vec3 tMaxVec = max(t1, t2);

    tMin = max(max(tMinVec.x, tMinVec.y), tMinVec.z);
    tMax = min(min(tMaxVec.x, tMaxVec.y), tMaxVec.z);

    return tMin <= tMax && tMax > 0.0;
}

float epsilon = 0.00001;

#define SIZE 256

vec3 CalculateNormals(vec3 hitPosition, vec3 minBounds, vec3 maxBounds)
{
    vec3 faceNormal;

    // Check which face has been hit
    vec3 center = (minBounds + maxBounds) * 0.5;
    vec3 extent = (maxBounds - minBounds) * 0.5;
    vec3 hitDifference = (hitPosition - center) / extent;

    vec3 absDifference = abs(hitDifference);

    // Determine the face normal based on the maximum difference
    faceNormal = vec3(
        step(absDifference.y, absDifference.x) * step(absDifference.z, absDifference.x),
        step(absDifference.x, absDifference.y) * step(absDifference.z, absDifference.y),
        step(absDifference.x, absDifference.z) * step(absDifference.y, absDifference.z)
    );

    return faceNormal;
}

vec3 nextAxis(vec3 t)
{
    return vec3(
        step(t.x, t.y) * step(t.x, t.z),
        step(t.y, t.x) * step(t.y, t.z),
        step(t.z, t.x) * step(t.z, t.y)
    );
}

struct HitInfo
{
    vec3 hitPos;
    vec3 normal;
    vec3 color;
    bool hit;
};

float noise(vec3 x)
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);
    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix( fract(sin(n) * 43758.5453), 
                        fract(sin(n + 1.0) * 43758.5453), f.x),
                    mix( fract(sin(n + 157.0) * 43758.5453), 
                        fract(sin(n + 158.0) * 43758.5453), f.x), f.y),
                mix(mix( fract(sin(n + 113.0) * 43758.5453), 
                        fract(sin(n + 114.0) * 43758.5453), f.x),
                    mix( fract(sin(n + 270.0) * 43758.5453), 
                        fract(sin(n + 271.0) * 43758.5453), f.x), f.y), f.z);
}

HitInfo TraverseVolume(Ray ray, float MaxRayDistance)
{
    vec3 volumeSize = vec3(SIZE);
    vec3 color = vec3(0.0);

    vec3 t1, t2;
    float tMin, tMax;
    if (!intersectRayAABB(ray, vec3(0.0), volumeSize, t1, t2, tMin, tMax))
        return HitInfo(vec3(0.0), vec3(0.0), color, false);

    // DDA algorithm
    vec3 rayStart = ray.origin + ray.direction * max(tMin+0.001, 0.0);
    vec3 step = sign(ray.direction);
    ivec3 currentPos = ivec3(rayStart);
    vec3 tDelta = 1.0 / ray.direction;
    vec3 t = abs((currentPos + max(step, 0.0) - rayStart) / ray.direction);

    while (true)
    {
        // Check if we are out of bounds if yes break the while loop
        if (currentPos.x < 0 || currentPos.y < 0 || currentPos.z < 0 || currentPos.x >= SIZE || currentPos.y >= SIZE || currentPos.z >= SIZE)
            break;
        
        int current = int(texelFetch(iVolume, currentPos, 0).r * 255.0);
        if (current == 0) { // air
            color += vec3(0.004, 0.0, 0.0);
            vec3 axis = nextAxis(t);
            t += tDelta * step * axis;
            currentPos += ivec3(step * axis);
        } else { // solid
            float noise = noise(currentPos) * 0.5 + 0.5;
            vec3 brownColor = vec3(0.71, 0.36, 0.13);
            vec3 brownColor2 = vec3(0.41, 0.18, 0.02);
            vec3 hitPos = rayStart + t * ray.direction;
            return HitInfo(
                hitPos,
                (CalculateNormals(hitPos, currentPos, currentPos + 1)),
                mix(brownColor2, brownColor, noise),
                true
            );
        }
    }
    return HitInfo(
        vec3(0.0),
        vec3(0.0),
        color,
        false
    );
}

void main()
{
    vec2 fragCoord = uv * iResolution;
    
    //camera setup
	float fov = radians(70.0);
	float fx = tan(fov / 2.0) / iResolution.x;
	vec2 d = fx * (fragCoord.xy * 2.0 - iResolution.xy);
    // Initial camera direction (z-axis)
	vec3 camDir = normalize(vec3(d.x, d.y, 1.0));
    // Apply view matrix to camDir
    camDir = (iViewMatrix * vec4(camDir, 0.0)).xyz;
    // get pos from view matrix
    vec3 camPos = (iViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;

    // TEMP TRANSFORM MATRIX
    // camPos = (transform * vec4(camPos, 1.0)).xyz;
    // camDir = (transform * vec4(camDir, 0.0)).xyz;
    //

	Ray cameraRay = Ray(camPos, camDir);

	//final color
    HitInfo hit = TraverseVolume(cameraRay, 150.0);
    // Sun
    vec3 sunPos = vec3(1024, 1024, 1024);
    vec3 sunDir = normalize(sunPos - hit.hitPos);
    float NormalLightDot = dot(hit.color, sunDir);
    float brightness = max(NormalLightDot, 0.2) * 1.5;
    vec3 diffuseColor = brightness * abs(hit.color);

    gAlbedo = vec4(diffuseColor, 1.0); // Albdeo
    gNormal = vec4(hit.normal, 1.0); // Normal
    gPosition = vec4(hit.hitPos / 255, 1.0); // Position
    gDepth = mix(vec4(1.0), vec4(vec3(length(hit.hitPos - camPos) / 400), 1.0), int(hit.hit)); // Depth FAR == 400
}