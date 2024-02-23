#version 430 core

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Node {
    int data;
    int children[8];
};

struct NodeInfo {
    int index;
    int childIdx;
    vec3 root;
    int depth;
    vec3 t1;
    vec3 t2;
} stack[64];

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iViewMatrix;

out vec4 out_Pixel;

layout(std430, binding = 0) readonly buffer SSBO
{
    Node SVO[];
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

vec3 CalculateNormals(vec3 hit, vec3 minBounds, vec3 maxBounds) {
    float epsilon = 0.0001;
    vec3 normal = step(minBounds + epsilon, hit) - step(hit, maxBounds - epsilon);
    return normalize(normal);
}

#define SIZE 128.0

int entryNode(vec3 t1, vec3 tm) {
    float Ax = 2.0 * step(tm.y, t1.x) + 4.0 * step(tm.z, t1.x);
    float Ay = 1.0 * step(tm.x, t1.y) + 4.0 * step(tm.z, t1.y);
    float Az = 1.0 * step(tm.x, t1.z) + 2.0 * step(tm.y, t1.z);
    return int (
        mix(mix(Az, Ay, step(t1.z, t1.y)), Ax, step(t1.y, t1.x) * step(t1.z, t1.x))
    );
}

int nextNode(vec3 t, int x, int y, int z) {
    return int (
        mix(mix(z, y, step(t.y, t.z)), x, step(t.x, t.y) * step(t.x, t.z))
    );
}

void fixRayNegative(Ray ray, out Ray positiveRay, out int a) {
    float stepX = 1 - step(0.0, ray.direction.x);
    positiveRay.origin.x = mix(ray.origin.x, SIZE - ray.origin.x, stepX);
    positiveRay.direction.x = mix(ray.direction.x, -ray.direction.x, stepX);
    a |= int(stepX * 1);
    
    float stepY = 1 - step(0.0, ray.direction.y);
    positiveRay.origin.y = mix(ray.origin.y, SIZE - ray.origin.y, stepY);
    positiveRay.direction.y = mix(ray.direction.y, -ray.direction.y, stepY);
    a |= int(stepY * 2);

    float stepZ = 1 - step(0.0, ray.direction.z);
    positiveRay.origin.z = mix(ray.origin.z, SIZE - ray.origin.z, stepZ);
    positiveRay.direction.z = mix(ray.direction.z, -ray.direction.z, stepZ);
    a |= int(stepZ * 4);
}

int tableOctree[8][3] = int[8][3](
    int[3](1, 2, 4),
    int[3](-1, 3, 5),
    int[3](3, -1, 6),
    int[3](-1, -1, 7),
    int[3](5, 6, -1),
    int[3](-1, 7, -1),
    int[3](7, -1, -1),
    int[3](-1, -1, -1)
);


vec3 TraverseOctree(Ray ray)
{
    vec3 octreeSize = vec3(SIZE);
    vec3 color = vec3(0.0);

    int a = 0;
    Ray positiveRay = ray;
    fixRayNegative(ray, positiveRay, a);

    vec3 t1, t2;
    float tMin, tMax;
    if (!intersectRayAABB(positiveRay, vec3(0.0), octreeSize, t1, t2, tMin, tMax))
        return vec3(0.0);

    int stackIndex = 0;
    stack[stackIndex++] = NodeInfo(0, 0, vec3(0.0), 0, t1, t2);

    while (stackIndex > 0)
    {
        NodeInfo current = stack[--stackIndex];

        float mySize = SIZE / (1 << current.depth);
        vec3 minBounds = vec3(  current.root.x + mySize * (current.childIdx & 1),
                                current.root.y + mySize * ((current.childIdx & 2) >> 1),
                                current.root.z + mySize * ((current.childIdx & 4) >> 2));

        // one LOD level every 100 distance
        vec3 center = minBounds + mySize / 2.0;
        float distance = length(center - ray.origin);
        int lod = 9 - int(distance / 100.0);

        Node currentNode = SVO[current.index];
        // Branch
        if ((currentNode.data & (1 << 31)) == 0 && current.depth < lod) {
            color += vec3(0.02, 0.0, 0.0);

            vec3 tm = 0.5 * (current.t1 + current.t2);

            int count = 0;
            NodeInfo tmpStack[4]; // because we push back but here we want to push front
            int currentNodeIndex = entryNode(current.t1, tm);
            while (currentNodeIndex >= 0)
            {
                vec3 nextT1 = vec3(
                    mix(current.t1.x, tm.x, float(currentNodeIndex & 1)),
                    mix(current.t1.y, tm.y, float((currentNodeIndex & 2) >> 1)),
                    mix(current.t1.z, tm.z, float((currentNodeIndex & 4) >> 2)));
                vec3 nextT2 = vec3(
                    mix(tm.x, current.t2.x, float(currentNodeIndex & 1)),
                    mix(tm.y, current.t2.y, float((currentNodeIndex & 2) >> 1)),
                    mix(tm.z, current.t2.z, float((currentNodeIndex & 4) >> 2)));
                tmpStack[count++] = NodeInfo(currentNode.children[currentNodeIndex^a], currentNodeIndex^a, minBounds, current.depth + 1, nextT1, nextT2);
                currentNodeIndex = nextNode(nextT2, tableOctree[currentNodeIndex][0], tableOctree[currentNodeIndex][1], tableOctree[currentNodeIndex][2]);
            }
            for (int i = count - 1; i >= 0; i--) {
                stack[stackIndex++] = tmpStack[i];
            }
        } else if ((currentNode.data & 1) != 0) { // Leaf is solid
            float tClosest = max(max(current.t1.x, current.t1.y), current.t1.z);
            return abs(CalculateNormals(ray.origin + ray.direction * tClosest, minBounds, minBounds + vec3(mySize)));
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
    vec3 color = TraverseOctree(cameraRay);
	out_Pixel = vec4(color, 1.0);
}