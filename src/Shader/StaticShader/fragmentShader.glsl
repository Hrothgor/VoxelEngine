#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

uniform mat4 iViewMatrix;

out vec4 out_Pixel;

struct Node {
    int data;
    int children[8];
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

layout(std430, binding = 0) readonly buffer SSBO
{
    Node SVO[];
};

bool intersectRayAABB(Ray ray, vec3 minBounds, vec3 maxBounds, out float tMin, out float tMax) {
    vec3 invDirection = 1.0 / ray.direction;
    vec3 t1 = (minBounds - ray.origin) * invDirection;
    vec3 t2 = (maxBounds - ray.origin) * invDirection;

    vec3 tMinVec = min(t1, t2);
    vec3 tMaxVec = max(t1, t2);

    tMin = max(max(tMinVec.x, tMinVec.y), tMinVec.z);
    tMax = min(min(tMaxVec.x, tMaxVec.y), tMaxVec.z);

    return tMin <= tMax && tMax > 0.0;
}

vec3 CalculateNormals(vec3 hit, vec3 minBounds, vec3 maxBounds) {
    float epsilon = 0.0001;
    vec3 normal = step(minBounds + epsilon, hit) - step(hit, maxBounds - epsilon);
    return normalize(normal);
}

struct NodeInfo {
    int index;
    vec3 root;
    int depth;
} stack[128];

#define SIZE 128

float rand(float x){
    return fract(sin(x) * 43758.5453);
}

// Traverse my SVO and find the closest intersection
vec3 TraverseOctree(Ray ray, out float hit)
{
    vec3 color = vec3(0.0);

    int stackIndex = 0;
    stack[stackIndex++] = NodeInfo(0, vec3(0.0), 0);

    float closestHit = 1000000.0;
    NodeInfo closestNodeInfo = NodeInfo(-1, vec3(0.0), 0);
    int closestLod = 7;

    while (stackIndex > 0)
    {
        NodeInfo currentInfo = stack[--stackIndex];

        float mySize = SIZE / (1 << currentInfo.depth);
        // Calculate my bounds from depth and root
        vec3 minBounds = currentInfo.root;
        vec3 maxBounds = currentInfo.root + vec3(mySize);

        float tMin, tMax;
        if (!intersectRayAABB(ray, minBounds, maxBounds, tMin, tMax))
            continue;

        // LOD every 50 distance
        vec3 center = minBounds + (maxBounds - minBounds) / 2.0;
        float distance = length(center - ray.origin);
        // int lod = 7 - int(distance / 50.0);
        int lod = 7;

        color += vec3(0.01, 0.0, 0.0);

        Node currentNode = SVO[currentInfo.index];
        if ((currentNode.data & (1 << 31)) == 0 && currentInfo.depth < lod)
        {
            stack[stackIndex++] = NodeInfo(currentNode.children[0], currentInfo.root, currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[1], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y, currentInfo.root.z), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[2], vec3(currentInfo.root.x, currentInfo.root.y + mySize / 2.0, currentInfo.root.z), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[3], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y + mySize / 2.0, currentInfo.root.z), currentInfo.depth + 1);

            stack[stackIndex++] = NodeInfo(currentNode.children[4], vec3(currentInfo.root.x, currentInfo.root.y, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[5], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[6], vec3(currentInfo.root.x, currentInfo.root.y + mySize / 2.0, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[7], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y + mySize / 2.0, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
        } else {
            if (tMin < closestHit && (currentNode.data & 1) != 0) {
                closestHit = tMin;
                closestNodeInfo = currentInfo;
                closestLod = lod;
            }
        }
    }

    if (closestNodeInfo.index != -1) {
        hit = closestHit;
        // return vec3(rand(closestLod*8), rand(closestLod+5), rand(closestLod+8));
        return abs(CalculateNormals(ray.origin + ray.direction * closestHit, closestNodeInfo.root, closestNodeInfo.root + vec3(SIZE / (1 << closestNodeInfo.depth))));
    }
    else 
        return color;
}

bool TraverseOctreeSun(Ray ray)
{
    int stackIndex = 0;
    stack[stackIndex++] = NodeInfo(0, vec3(0.0), 0);

    float closestHit = 1000000.0;
    NodeInfo closestNodeInfo = NodeInfo(-1, vec3(0.0), 0);
    
    while (stackIndex > 0)
    {
        NodeInfo currentInfo = stack[--stackIndex];

        float mySize = SIZE / (1 << currentInfo.depth);
        vec3 minBounds = currentInfo.root;
        vec3 maxBounds = currentInfo.root + vec3(mySize);

        float tMin, tMax;
        if (!intersectRayAABB(ray, minBounds, maxBounds, tMin, tMax))
            continue;

        Node currentNode = SVO[currentInfo.index];
        if ((currentNode.data & (1 << 31)) == 0)
        {
            stack[stackIndex++] = NodeInfo(currentNode.children[0], currentInfo.root, currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[1], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y, currentInfo.root.z), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[2], vec3(currentInfo.root.x, currentInfo.root.y + mySize / 2.0, currentInfo.root.z), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[3], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y + mySize / 2.0, currentInfo.root.z), currentInfo.depth + 1);

            stack[stackIndex++] = NodeInfo(currentNode.children[4], vec3(currentInfo.root.x, currentInfo.root.y, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[5], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[6], vec3(currentInfo.root.x, currentInfo.root.y + mySize / 2.0, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
            stack[stackIndex++] = NodeInfo(currentNode.children[7], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y + mySize / 2.0, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1);
        } else {
            if ((currentNode.data & 1) != 0) {
                return true;
            }
        }
    }
    return false;
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
    float hit;
    vec3 color = TraverseOctree(cameraRay, hit);
    // if (hit > 0.0) {
    //     vec3 sunPos = vec3(512);
    //     sunPos.x = cos(iTime) * 512;
    //     sunPos.z = sin(iTime) * 512;
    //     cameraRay.origin += cameraRay.direction * (hit - 0.01);
    //     cameraRay.direction = normalize(sunPos - cameraRay.origin);
    //     if (TraverseOctreeSun(cameraRay))
    //         color *= 0.5;
    // }
	out_Pixel = vec4(color, 1.0);
}