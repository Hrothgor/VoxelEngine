#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iViewMatrix;
uniform mat4 iTransformMatrix;

uniform sampler3D iVolume;
uniform sampler2D iMaterialPalette;

layout (location = 0) out vec4 gAlbedo;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
layout (location = 3) out vec4 gDepth;

struct Ray {
    vec3 origin;
    vec3 direction;
};

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

HitInfo TraverseVolume(Ray ray, float MaxRayDistance)
{
    vec3 volumeSize = textureSize(iVolume, 0);

    vec3 t1, t2;
    float tMin, tMax;
    if (!intersectRayAABB(ray, vec3(0.0), volumeSize, t1, t2, tMin, tMax))
        return HitInfo(vec3(0.0), vec3(0.0), vec3(0.0), false);

    // DDA algorithm
    vec3 rayStart = ray.origin + ray.direction * max(tMin+0.001, 0.0);
    vec3 hitPos = rayStart;
    ivec3 currentPos = ivec3(rayStart);
    vec3 step = sign(ray.direction);
    vec3 invDir = 1.0 / ray.direction;
    vec3 tDelta = invDir * step;
    vec3 t = abs((currentPos + max(step, 0.0) - rayStart) * invDir);
    vec3 oldAxis = vec3(0.0);

    int mipmapLevel = 0;

    while (length(hitPos - ray.origin) < MaxRayDistance)
    {
        vec3 bounds = vec3(0.0) + (volumeSize / (1 << mipmapLevel));
        // Check if we are out of bounds if yes break the while loop
        if (currentPos.x < 0 || currentPos.y < 0 || currentPos.z < 0
        || currentPos.x >= bounds.x || currentPos.y >= bounds.y || currentPos.z >= bounds.z)
            break;
        
        int current = int(texelFetch(iVolume, (currentPos / (1 << mipmapLevel)), mipmapLevel).r * 255.0);
        if (current == 0) { // air
            hitPos = rayStart + min(t.x, min(t.y, t.z)) * ray.direction;
            vec3 axis = nextAxis(t);
            oldAxis = axis;
            t += tDelta * axis;
            currentPos += ivec3(step * axis);
        } else { // solid
            vec3 color = vec3(
                texelFetch(iMaterialPalette, ivec2(0, current - 1), 0).r,
                texelFetch(iMaterialPalette, ivec2(1, current - 1), 0).r,
                texelFetch(iMaterialPalette, ivec2(2, current - 1), 0).r
            );
            return HitInfo(
                hitPos,
                -oldAxis * step,
                color,
                true
            );
        }
    }
    return HitInfo(
        vec3(0.0),
        vec3(0.0),
        vec3(0.0),
        false
    );
}

Ray CreateRayFromCamera(float fov)
{
    vec2 fragCoord = uv * iResolution;
    //camera setup
	float rfov = radians(fov);
	float fx = tan(rfov / 2.0) / iResolution.x;
	vec2 d = fx * (fragCoord.xy * 2.0 - iResolution.xy);
    // Initial camera direction (z-axis)
	vec3 camDir = normalize(vec3(d.x, d.y, 1.0));
    // Apply view matrix to camDir
    camDir = (iViewMatrix * vec4(camDir, 0.0)).xyz;
    camDir = (iTransformMatrix * vec4(camDir, 0.0)).xyz;
    // get pos from view matrix
    vec3 camPos = (iViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    camPos = (iTransformMatrix * vec4(camPos, 1.0)).xyz;

    return Ray(camPos, camDir);
}

void main()
{
	Ray ray = CreateRayFromCamera(70.0);

    HitInfo hit = TraverseVolume(ray, 300.0);

    gAlbedo = vec4(hit.color, 1.0); // Albdeo
    gNormal = vec4(hit.normal, 1.0); // Normal
    gPosition = vec4(hit.hitPos, 1.0); // Position
    gDepth = mix(vec4(1.0), vec4(vec3(length(hit.hitPos - ray.origin) / 400), 1.0), int(hit.hit)); // Depth FAR == 400
}