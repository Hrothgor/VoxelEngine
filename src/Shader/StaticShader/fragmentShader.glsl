#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

// camera
uniform mat4 iViewMatrix;

layout(std430, binding = 3) readonly buffer SSBO
{
    uint SVO[];
};

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

vec3 CalculateNormals(vec3 hit, vec3 minBounds, vec3 maxBounds) {
    float epsilon = 0.001;
    vec3 normal = step(minBounds + epsilon, hit) - step(hit, maxBounds - epsilon);
    return normalize(normal);
}

struct NodeInfo {
    int index;
    vec3 root;
    int depth;
} stack[64];

// Traverse my SVO and find the closest intersection
vec3 TraverseOctree(Ray ray)
{
    int stackIndex = 0;
    stack[stackIndex++] = NodeInfo(0, vec3(0.0), 0);

    
    while (stackIndex >= 0)
    {
        NodeInfo current = stack[--stackIndex];
        float tMin, tMax;
        vec3 minBounds = vec3(current.root);
        vec3 maxBounds = vec3(64.0);
        if (intersectRayAABB(ray, minBounds, maxBounds, tMin, tMax))
            return abs(CalculateNormals(ray.origin + ray.direction * tMin, minBounds, maxBounds));
        else 
            return vec3(0.0);

        stackIndex--;
    }



    // while (stackIndex >= 0)
    // {
    //     NodeInfo current = stack[stackIndex--];

    //     float mySize = 64.0 / (1 << current.depth);
    //     // Calculate my bounds from depth and root
    //     vec3 minBounds = current.root;
    //     vec3 maxBounds = current.root + vec3(mySize);

    //     float tMin, tMax;
    //     if (!intersectRayAABB(ray, minBounds, maxBounds, tMin, tMax))
    //         continue;
        
    //     // Traverse the octree, if a node is a 0 it means it's a branch and the next 8 nodes are the children
    //     // for (int i = 0; i < 1; i++)
    //     // {
    //         uint node = SVO[current.index];
    //         bool isBranch = (node & (1 << 31)) == 0;
    //         if (isBranch)
    //         {
    //             if (intersectRayAABB(ray, minBounds, maxBounds, tMin, tMax))
    //                 return abs(CalculateNormals(ray.origin + ray.direction * tMin, minBounds, maxBounds));
    //             // Traverse all 8 children
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, current.root, current.depth + 1);
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x + mySize / 2.0, current.root.y, current.root.z), current.depth + 1);
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x, current.root.y + mySize / 2.0, current.root.z), current.depth + 1);
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x + mySize / 2.0, current.root.y + mySize / 2.0, current.root.z), current.depth + 1);

    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x, current.root.y, current.root.z + mySize / 2.0), current.depth + 1);
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x + mySize / 2.0, current.root.y, current.root.z + mySize / 2.0), current.depth + 1);
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x, current.root.y + mySize / 2.0, current.root.z + mySize / 2.0), current.depth + 1);
    //             // stack[stackIndex++] = NodeInfo(current.index + 1, vec3(current.root.x + mySize / 2.0, current.root.y + mySize / 2.0, current.root.z + mySize / 2.0), current.depth + 1);
    //         }
    //         // Check if first byte is 1, if so it's a leaf node
    //         else if ((node & 1) == 1)
    //         {
    //             // Leaf node, check for intersection
    //             if (intersectRayAABB(ray, minBounds, maxBounds, tMin, tMax))
    //                 return abs(CalculateNormals(ray.origin + ray.direction * tMin, minBounds, maxBounds));
    //         }
    //     // }
    // }

    // return vec3(0.0);
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
	out_Pixel = vec4(TraverseOctree(cameraRay), 1.0);
}































// float rand(float n){return fract(sin(n) * 43758.5453123) * 2 - 1;}

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