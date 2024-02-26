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

out vec4 out_Pixel;

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

float epsilon = 0.0001;

#define SIZE 256

bool isEqual(float a, float b)
{
    return abs(a - b) < epsilon;
}

vec3 CalculateNormals(vec3 hit, vec3 minBounds)
{
    vec3 normal = step(minBounds + epsilon, hit) - step(hit, minBounds + 1 - epsilon);
    return normalize(normal);
}

vec3 nextAxis(vec3 t)
{
    return vec3(
        step(t.x, t.y) * step(t.x, t.z),
        step(t.y, t.x) * step(t.y, t.z),
        step(t.z, t.x) * step(t.z, t.y)
    );
}

vec3 TraverseVolume(Ray ray, float MaxRayDistance, out vec3 hitPos)
{
    vec3 volumeSize = vec3(SIZE);
    vec3 color = vec3(0.0);

    vec3 t1, t2;
    float tMin, tMax;
    if (!intersectRayAABB(ray, vec3(0.0), volumeSize, t1, t2, tMin, tMax))
        return vec3(0.0);

    // DDA algorithm
    vec3 rayStart = ray.origin + ray.direction * max(tMin, 0.0);
    vec3 step = sign(ray.direction);
    ivec3 currentPos = ivec3(rayStart + epsilon * ray.direction);
    vec3 tDelta = 1.0 / ray.direction * step;
    vec3 t = abs((currentPos + max(step, 0.0) - rayStart) / ray.direction);
    vec3 tBegin = abs((currentPos + max(step, 0.0) - rayStart) / ray.direction);

    while (true)
    {
        // Check if we are out of bounds if yes break the while loop
        if (currentPos.x < 0 || currentPos.y < 0 || currentPos.z < 0 || currentPos.x >= SIZE || currentPos.y >= SIZE || currentPos.z >= SIZE)
            break;
        
        int current = int(texelFetch(iVolume, currentPos, 0).r * 255.0);
        if (current == 0) { // air
            color += vec3(0.004, 0.0, 0.0);
            vec3 axis = nextAxis(t);
            t += tDelta * axis;
            currentPos += ivec3(step * axis);
        } else { // solid
            hitPos = rayStart + ray.direction * t;
            vec3 axis = nextAxis(t);
            return axis;
        }
    }
    return color;
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
	Ray cameraRay = Ray(camPos, camDir);

	//final color
    vec3 hitPos;
    vec3 color = TraverseVolume(cameraRay, 150.0, hitPos);
    // Sun
    vec3 sunPos = vec3(1024, 1024, 1024);
    vec3 sunDir = normalize(sunPos - hitPos);
    float NormalLightDot = dot(color, sunDir);
    float brightness = max(NormalLightDot, 0.2) * 1.5;
    vec3 diffuseColor = brightness * abs(color);

	out_Pixel = vec4(diffuseColor, 1.0);
}