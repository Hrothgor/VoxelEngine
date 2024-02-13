#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

uniform mat4 iViewMatrix;

out vec4 out_Pixel;

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
    vec3 root;
    int depth;
    vec3 t1;
    vec3 t2;
    int a;
} stack[128];

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

float rand(float x){
    return fract(sin(x) * 43758.5453);
}

// Traverse my SVO and find the closest intersection
vec3 TraverseOctree(Ray ray)
{
    vec3 color = vec3(0.0);

    int stackIndex = 0;
    stack[stackIndex++] = NodeInfo(0, vec3(0.0), 0, vec3(0.0), vec3(0.0), 0);

    float closestHit = 1000000.0;
    NodeInfo closestNodeInfo = NodeInfo(-1, vec3(0.0), 0, vec3(0.0), vec3(0.0), 0);
    int closestLod = 7;

    while (stackIndex > 0)
    {
        NodeInfo currentInfo = stack[--stackIndex];

        float mySize = SIZE / (1 << currentInfo.depth);
        // Calculate my bounds from depth and root
        vec3 minBounds = currentInfo.root;
        vec3 maxBounds = currentInfo.root + vec3(mySize);

        vec3 t1, t2;
        float tMin, tMax;
        if (!intersectRayAABB(ray, minBounds, maxBounds, t1, t2, tMin, tMax))
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
            stack[stackIndex++] = NodeInfo(currentNode.children[0], currentInfo.root, currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
            stack[stackIndex++] = NodeInfo(currentNode.children[1], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y, currentInfo.root.z), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
            stack[stackIndex++] = NodeInfo(currentNode.children[2], vec3(currentInfo.root.x, currentInfo.root.y + mySize / 2.0, currentInfo.root.z), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
            stack[stackIndex++] = NodeInfo(currentNode.children[3], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y + mySize / 2.0, currentInfo.root.z), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);

            stack[stackIndex++] = NodeInfo(currentNode.children[4], vec3(currentInfo.root.x, currentInfo.root.y, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
            stack[stackIndex++] = NodeInfo(currentNode.children[5], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
            stack[stackIndex++] = NodeInfo(currentNode.children[6], vec3(currentInfo.root.x, currentInfo.root.y + mySize / 2.0, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
            stack[stackIndex++] = NodeInfo(currentNode.children[7], vec3(currentInfo.root.x + mySize / 2.0, currentInfo.root.y + mySize / 2.0, currentInfo.root.z + mySize / 2.0), currentInfo.depth + 1, vec3(0.0), vec3(0.0), 0);
        } else {
            if (tMin < closestHit && (currentNode.data & 1) != 0) {
                closestHit = tMin;
                closestNodeInfo = currentInfo;
                closestLod = lod;
            }
        }
    }

    if (closestNodeInfo.index != -1) {
        // return vec3(rand(closestLod*8), rand(closestLod+5), rand(closestLod+8));
        return abs(CalculateNormals(ray.origin + ray.direction * closestHit, closestNodeInfo.root, closestNodeInfo.root + vec3(SIZE / (1 << closestNodeInfo.depth))));
    }
    else 
        return color;
}






int entryNode(vec3 t1, vec3 tm) {
    int answer = 0;
    // select the entry plane and set bits
    if (t1.x > t1.y) {
        if(t1.x > t1.z) { // PLANE YZ because t1.x is maximum
            if (tm.y < t1.x) answer |= 2; // set bit at position 1
            if (tm.z < t1.x) answer |= 4; // set bit at position 2
            return answer;
        }
    }
    else if (t1.y > t1.z) { // PLANE XZ because t1.y is maximum
        if (tm.x < t1.y) answer |= 1; // set bit at position 0
        if (tm.z < t1.y) answer |= 4; // set bit at position 2
        return answer;
    }
    // PLANE XY because t1.z is maximum
    if (tm.x < t1.z) answer |= 1; // set bit at position 0
    if (tm.y < t1.z) answer |= 2; // set bit at position 1
    return answer;
}

int nextNode(vec3 t, int x, int y, int z) {
    if (t.x < t.y) {
        if (t.x < t.z) 
            return x;  // YZ plane because t.x is minimum
    }
    else if (t.y < t.z) {
        return y; // XZ plane because t.y is minimum
    }
    return z; // XY plane because t.z is minimum
}

vec3 EfficientTraverseOctree(Ray ray)
{
    int a = 0;
    vec3 octreeSize = vec3(SIZE);

    Ray positiveRay = ray;
    // fixes for rays with negative direction
    float stepX = 1 - step(0.0, ray.direction.x);
    positiveRay.origin.x = stepX * (octreeSize.x - ray.origin.x) + (1 - stepX) * ray.origin.x;
    positiveRay.direction.x = stepX * -ray.direction.x + (1 - stepX) * ray.direction.x;
    a |= int(stepX * 1);

    float stepY = 1 - step(0.0, ray.direction.y);
    positiveRay.origin.y = stepY * (octreeSize.y - ray.origin.y) + (1 - stepY) * ray.origin.y;
    positiveRay.direction.y = stepY * -ray.direction.y + (1 - stepY) * ray.direction.y;
    a |= int(stepY * 2);

    float stepZ = 1 - step(0.0, ray.direction.z);
    positiveRay.origin.z = stepZ * (octreeSize.z - ray.origin.z) + (1 - stepZ) * ray.origin.z;
    positiveRay.direction.z = stepZ * -ray.direction.z + (1 - stepZ) * ray.direction.z;
    a |= int(stepZ * 4);

    vec3 color = vec3(0.0);

    vec3 t1, t2;
    float tMin, tMax;
    if (!intersectRayAABB(positiveRay, vec3(0.0), octreeSize, t1, t2, tMin, tMax))
        return vec3(0.0);

    int stackIndex = 0;
    stack[stackIndex++] = NodeInfo(0, vec3(0.0), 0, t1, t2, 0);

    while (stackIndex > 0)
    {
        NodeInfo current = stack[--stackIndex];

        if (current.t2.x < 0 || current.t2.y < 0 || current.t2.z < 0) continue;

        float mySize = SIZE / (1 << current.depth);
        vec3 minBounds = vec3(current.root.x + mySize * (current.a & 1), current.root.y + mySize * ((current.a & 2) >> 1), current.root.z + mySize * ((current.a & 4) >> 2));
        vec3 maxBounds = minBounds + vec3(mySize);

        Node currentNode = SVO[current.index];
        // Branch
        if ((currentNode.data & (1 << 31)) == 0) {
            color += vec3(0.02);

            vec3 tm = 0.5 * (current.t1 + current.t2);

            int count = 0;
            NodeInfo tmpStack[4]; // because we push back but here we want to push front
            int currentNodeIndex = entryNode(current.t1, tm);
            while (currentNodeIndex >= 0)
            {
                switch (currentNodeIndex)
                {
                case 0: {
                    tmpStack[count++] = NodeInfo(currentNode.children[a],
                                        minBounds, current.depth + 1,
                                        current.t1, tm, a);
                    currentNodeIndex = nextNode(vec3(tm.x, tm.y, tm.z), 1, 2, 4);
                    break;}
                case 1: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[1^a],
                                        minBounds, current.depth + 1,
                                        vec3(tm.x, current.t1.y, current.t1.z), vec3(current.t2.x, tm.y, tm.z), 1^a);
                    currentNodeIndex = nextNode(vec3(current.t2.x, tm.y, tm.z), -1, 3, 5);
                    break;}
                case 2: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[2^a],
                                        minBounds, current.depth + 1,
                                        vec3(current.t1.x, tm.y, current.t1.z), vec3(tm.x, current.t2.y, tm.z), 2^a);
                    currentNodeIndex = nextNode(vec3(tm.x, current.t2.y, tm.z), 3, -1, 6);
                    break;}
                case 3: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[3^a],
                                        minBounds, current.depth + 1,
                                        vec3(tm.x, tm.y, current.t1.z), vec3(current.t2.x, current.t2.y, tm.z), 3^a);
                    currentNodeIndex = nextNode(vec3(current.t2.x, current.t2.y, tm.z), -1, -1, 7);
                    break;}
                case 4: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[4^a],
                                        minBounds, current.depth + 1,
                                        vec3(current.t1.x, current.t1.y, tm.z), vec3(tm.x, tm.y, current.t2.z), 4^a);
                    currentNodeIndex = nextNode(vec3(tm.x, tm.y, current.t2.z), 5, 6, -1);
                    break;}
                case 5: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[5^a],
                                        minBounds, current.depth + 1,
                                        vec3(tm.x, current.t1.y, tm.z), vec3(current.t2.x, tm.y, current.t2.z), 5^a);
                    currentNodeIndex = nextNode(vec3(current.t2.x, tm.y, current.t2.z), -1, 7, -1);
                    break;}
                case 6: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[6^a],
                                        minBounds, current.depth + 1,
                                        vec3(current.t1.x, tm.y, tm.z), vec3(tm.x, current.t2.y, current.t2.z), 6^a);
                    currentNodeIndex = nextNode(vec3(tm.x, current.t2.y, current.t2.z), 7, -1, -1);
                    break;}
                case 7: { 
                    tmpStack[count++] = NodeInfo(currentNode.children[7^a],
                                        minBounds, current.depth + 1,
                                        tm, current.t2, 7^a);
                    currentNodeIndex = -1;
                    break;}
                }
            }
            for (int i = count - 1; i >= 0; i--) {
                stack[stackIndex++] = tmpStack[i];
            }
        } else if ((currentNode.data & 1) != 0) { // Leaf is solid
            float tClosest = max(max(current.t1.x, current.t1.y), current.t1.z);
            return abs(CalculateNormals(ray.origin + ray.direction * tClosest, minBounds, maxBounds));
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
    vec3 color = EfficientTraverseOctree(cameraRay);
	out_Pixel = vec4(color, 1.0);
}







// // void proc_subtree (double tx0, double ty0, double tz0, double tx1, double ty1, double tz1, Node* node){
// //     float txm, tym, tzm;

// void proc_subtree(vec3 t1, vec3 t2, NodeInfo nodeInfo) {

//     if (t2.x < 0 || t2.y < 0 || t2.z < 0) return;
//     // if (any(lessThan(tMax, vec3(0.0)))) return;
    


//     if (nodeInfo->IsLEAF){
//         return;
//     }

//     vec3 tm = (t1 + t2) * 0.5;
//     // txm = 0.5*(tx0 + tx1);
//     // tym = 0.5*(ty0 + ty1);
//     // tzm = 0.5*(tz0 + tz1);

//     int currentNodeIndex = entryNode(t1.x, t1.y, t1.z, tm.x,tm.y,tm.z);
//     do{
//         switch (currentNodeIndex)
//         {
//         case 0: { 
//             proc_subtree(tx0,ty0,tz0,txm,tym,tzm,node->children[a]);
//             // currentNodeIndex = new_node(txm,4,tym,2,tzm,1);
//             currentNodeIndex = nextNode(vec3(tm.x, tm.y, tm.z), 1, 2, 4);
//             break;}
//         case 1: { 
//             proc_subtree(tx0,ty0,tzm,txm,tym,tz1,node->children[1^a]);
//             // currentNodeIndex = new_node(txm,5,tym,3,tz1,8);
//             currentNodeIndex = nextNode(vec3(t2.x, tm.y, tm.z), -1, 3, 5);
//             break;}
//         case 2: { 
//             proc_subtree(tx0,tym,tz0,txm,ty1,tzm,node->children[2^a]);
//             // currentNodeIndex = new_node(txm,6,ty1,8,tzm,3);
//             currentNodeIndex = nextNode(vec3(tm.x, t2.y, tm.z), 3, -1, 6);
//             break;}
//         case 3: { 
//             proc_subtree(tx0,tym,tzm,txm,ty1,tz1,node->children[3^a]);
//             // currentNodeIndex = new_node(txm,7,ty1,8,tz1,8);
//             currentNodeIndex = nextNode(vec3(t2.x, t2.y, tm.z), -1, -1, 7);
//             break;}
//         case 4: { 
//             proc_subtree(txm,ty0,tz0,tx1,tym,tzm,node->children[4^a]);
//             // currentNodeIndex = new_node(tx1,8,tym,6,tzm,5);
//             currentNodeIndex = nextNode(vec3(tm.x, tm.y, t2.z), 5, 6, -1);
//             break;}
//         case 5: { 
//             proc_subtree(txm,ty0,tzm,tx1,tym,tz1,node->children[5^a]);
//             // currentNodeIndex = new_node(tx1,8,tym,7,tz1,8);
//             currentNodeIndex = nextNode(vec3(t2.x, tm.y, t2.z), -1, 7, -1);
//             break;}
//         case 6: { 
//             proc_subtree(txm,tym,tz0,tx1,ty1,tzm,node->children[6^a]);
//             // currentNodeIndex = new_node(tx1,8,ty1,8,tzm,7);
//             currentNodeIndex = nextNode(vec3(tm.x, t2.y, t2.z), 7, -1, -1);
//             break;}
//         case 7: { 
//             proc_subtree(txm,tym,tzm,tx1,ty1,tz1,node->children[7^a]);
//             // currentNodeIndex = 8;
//             currentNodeIndex = -1;
//             break;}
//         }
//     } while (currentNodeIndex>0);
// }