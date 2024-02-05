#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

// camera
uniform mat4 iViewMatrix;

out vec4 out_Pixel;

struct Ray {
    vec3 origin;
    vec3 direction;
};

bool intersectRayAABB(Ray ray, vec3 minBounds, vec3 maxBounds, out float tMin, out float tMax) {
    vec3 invDirection = 1.0 / ray.direction;
    vec3 t1 = (minBounds - ray.origin) * invDirection;
    vec3 t2 = (maxBounds - ray.origin) * invDirection;

    vec3 tMinVec = min(t1, t2);
    vec3 tMaxVec = max(t1, t2);

    tMin = max(max(tMinVec.x, tMinVec.y), tMinVec.z);
    tMax = min(min(tMaxVec.x, tMaxVec.y), tMaxVec.z);

    return tMin <= tMax;
}

float rand(float n){return fract(sin(n) * 43758.5453123) * 2 - 1;}

vec3 CalculateNormals(vec3 hit, vec3 minBounds, vec3 maxBounds) {
    float epsilon = 0.001;
    vec3 normal = step(minBounds + epsilon, hit) - step(hit, maxBounds - epsilon);
    return normalize(normal);
}

// Smooth raycast to add anti aliasing (using multiple intersectRayAABB calls around the pixel)
// vec3 raycastAABB(Ray ray, vec3 minBounds, vec3 maxBounds, int steps) {
//     float t = 0.0;

//     float t1, t2;
//     if (!intersectRayAABB(ray, minBounds, maxBounds, t1, t2))
//         return vec3(0.15, 0.6, 0.2);


//     vec3 normal = vec3(0.0);
//     Ray jitteredRay = ray;
//     for (int i = 0; i < steps; i++) {
//         float tMin, tMax;
//         t += float(intersectRayAABB(jitteredRay, minBounds, maxBounds, tMin, tMax));
//         if (i == 0)
//             normal = CalculateNormals(jitteredRay.origin + jitteredRay.direction * tMin, minBounds, maxBounds);
//         jitteredRay = ray;
//         vec3 jitter = vec3(rand(float(i)), rand(float(i)), rand(float(i))) * 0.05;
//         jitteredRay.origin += jitter;
//     }
//     return (t / float(steps)) * normal;
// }

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

	Ray cameraRay = Ray(camPos, camDir);

	//final color
    vec3 color = vec3(0.0);
    vec3 cubeMinBounds = vec3(-1.0, -1.0, 6.0);
    vec3 cubeMaxBounds = vec3(1.0, 1.0, 8.0); 
    float t1, t2;
    if (!intersectRayAABB(cameraRay, cubeMinBounds, cubeMaxBounds, t1, t2))
        color = vec3(0.15, 0.4, 0.2);
    else
        color = abs(CalculateNormals(cameraRay.origin + cameraRay.direction * t1, cubeMinBounds, cubeMaxBounds));
	out_Pixel = vec4(color, 1.0);
}