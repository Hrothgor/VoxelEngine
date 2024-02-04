#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;

// camera
uniform vec3 iCamPos;
uniform vec3 iCamForward; // forward vector

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

// Smooth raycast to add anti aliasing (using multiple intersectRayAABB calls around the pixel)
float raycastAABB(Ray ray, vec3 minBounds, vec3 maxBounds, int steps) {
    float t = 0.0;

    Ray jitteredRay = ray;
    for (int i = 0; i < steps; i++) {
        float tMin, tMax;
        t += float(intersectRayAABB(jitteredRay, minBounds, maxBounds, tMin, tMax));
        jitteredRay = ray;
        vec3 jitter = vec3(rand(float(i)), rand(float(i)), rand(float(i))) * 0.01;
        jitteredRay.origin += jitter;
    }
    return t / float(steps);
}

// Rotation matrices
mat3 getRotationX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(1.0, 0.0, 0.0, 0.0, c, -s, 0.0, s, c);
}

mat3 getRotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat3(c, 0.0, s, 0.0, 1.0, 0.0, -s, 0.0, c);
}

mat4 CreateViewMatrix(vec3 cameraPosition, vec3 cameraForward, vec3 cameraUp) {
    vec3 zAxis = normalize(cameraForward);
    vec3 xAxis = normalize(cross(cameraUp, zAxis));
    vec3 yAxis = cross(zAxis, xAxis);

    mat4 viewMatrix = mat4(1.0);
    viewMatrix[0][0] = xAxis.x;
    viewMatrix[1][0] = xAxis.y;
    viewMatrix[2][0] = xAxis.z;
    viewMatrix[3][0] = -dot(xAxis, cameraPosition);

    viewMatrix[0][1] = yAxis.x;
    viewMatrix[1][1] = yAxis.y;
    viewMatrix[2][1] = yAxis.z;
    viewMatrix[3][1] = -dot(yAxis, cameraPosition);

    viewMatrix[0][2] = zAxis.x;
    viewMatrix[1][2] = zAxis.y;
    viewMatrix[2][2] = zAxis.z;
    viewMatrix[3][2] = -dot(zAxis, cameraPosition);

    return viewMatrix;
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
    mat4 viewMatrix = CreateViewMatrix(iCamPos, iCamForward, vec3(0.0, 1.0, 0.0));
    camDir = (viewMatrix * vec4(camDir, 0.0)).xyz;

	Ray cameraRay = Ray(iCamPos, camDir);

	//final color
	vec3 color = vec3(raycastAABB(cameraRay, vec3(-1.0, -1.0, 6.0), vec3(1.0, 1.0, 8.0), 10));
	out_Pixel = vec4(color, 1.0);
}














// int hexid;
// vec3 hpos, point, pt;
// float tcol, bcol, hitbol, hexpos, fparam=0.;

// mat2 rot(float a) {
//     float s=sin(a),c=cos(a);
//     return mat2(c,s,-s,c);
// }

// vec3 path(float t) {
//     return vec3(sin(t*.3+cos(t*.2)*.5)*4.,cos(t*.2)*3.,t);
// }

// float hexagon( in vec2 p, in float r )
// {
//     const vec3 k = vec3(-0.866025404,0.5,0.577350269);
//     p = abs(p);
//     p -= 2.0*min(dot(k.xy,p),0.0)*k.xy;
//     p -= vec2(clamp(p.x, -k.z*r, k.z*r), r);
//     return length(p)*sign(p.y);
// }

// float hex(vec2 p) {
//     p.x *= 0.57735*2.0;
// 	p.y+=mod(floor(p.x),2.0)*0.5;
// 	p=abs((mod(p,1.0)-0.5));
// 	return abs(max(p.x*1.5 + p.y, p.y*2.0) - 1.0);
// }

// mat3 lookat(vec3 dir) {
//     vec3 up=vec3(0.,1.,0.);
//     vec3 rt=normalize(cross(dir,up));
//     return mat3(rt, cross(rt,dir), dir);
// }

// float hash12(vec2 p)
// {
// 	p*=1000.;
// 	vec3 p3  = fract(vec3(p.xyx) * .1031);
//     p3 += dot(p3, p3.yzx + 33.33);
//     return fract((p3.x + p3.y) * p3.z);
// }

// float de(vec3 p) {
//     pt=vec3(p.xy-path(p.z).xy,p.z);
//     float h=abs(hexagon(pt.xy,3.+fparam));
//     hexpos=hex(pt.yz);
//     tcol=smoothstep(.0,.15,hexpos);
//     h-=tcol*.1;
//     vec3 pp=p-hpos;
//     pp=lookat(point)*pp;
//     pp.y-=abs(sin(iTime))*3.+(fparam-(2.-fparam));
//     pp.yz*=rot(-iTime);
//     float bola=length(pp)-1.;
//     bcol=smoothstep(0.,.5,hex(pp.xy*3.));
//     bola-=bcol*.1;
//     vec3 pr=p;
//     pr.z=mod(p.z,6.)-3.;
//     float d=min(h,bola);
//     if (d==bola) {
//         tcol=1.;
//         hitbol=1.;
//     }
//     else {
//         hitbol=0.;
//         bcol=1.;
//     }
//     return d*.5;
// }

// vec3 normal(vec3 p) {
//     vec2 e=vec2(0.,.005);
//     return normalize(vec3(de(p+e.yxx),de(p+e.xyx),de(p+e.xxy))-de(p));
// }

// vec3 march(vec3 from, vec3 dir) {
//     vec3 odir=dir;
//     vec3 p=from,col=vec3(0.);
//     float d,td=0.;
//     vec3 g=vec3(0.);
//     for (int i=0; i<200; i++) {
//         d=de(p);
//         if (d<.001||td>200.) break;
//         p+=dir*d;
//         td+=d;
//         g+=.1/(.1+d)*hitbol*abs(normalize(point));
//     }
//     float hp=hexpos*(1.-hitbol);
//     p-=dir*.01;
//     vec3 n=normal(p);
//     if (d<.001) {
//         col=pow(max(0.,dot(-dir,n)),2.)*vec3(.6,.7,.8)*tcol*bcol;
//     }
//     col+=float(hexid);
//     vec3 pr=pt;
//     dir=reflect(dir,n);
//     td=0.;
//     for (int i=0; i<200; i++) {
//         d=de(p);
//         if (d<.001||td>200.) break;
//         p+=dir*d;
//         td+=d;
//         g+=.1/(.1+d)*abs(normalize(point));
//     }
//     float zz=p.z;
//     if (d<.001) {
//         vec3 refcol=pow(max(0.,dot(-odir,n)),2.)*vec3(.6,.7,.8)*tcol*bcol;
//         p=pr;
//         p=abs(.5-fract(p*.1));
//         float m=100.;
//         for (int i=0; i<10; i++) {
//             p=abs(p)/dot(p,p)-.8;
//             m=min(m,length(p));
//         }
//         col=mix(col,refcol,m)-m*.8;
//         col+=step(.3,hp)*step(.9,fract(pr.z*.05+iTime*.5+hp*.1))*.4;
//         col+=step(.3,hexpos)*step(.9,fract(zz*.05+iTime+hexpos*.1))*.3;
//     }
//     col+=g*.03;
// 	col.rb*=rot(odir.y*.5);
// 	return col;
// }


// void main()
// {
//     vec2 uv = (uv * 2 - 800/600);
//     float t=iTime*2.;
//     vec3 from=path(t);
//     if (mod(iTime-10.,20.)>10.) {
//         from=path(floor(t/20.)*20.+10.);
//         from.x+=2.;
//     }
//     hpos=path(t+3.);
//     vec3 adv=path(t+2.);
//     vec3 dir=normalize(vec3(uv,.7));
//     vec3 dd=normalize(adv-from);
//     point=normalize(adv-hpos);
//     point.xz*=rot(sin(iTime)*.2);
//     dir=lookat(dd)*dir;
//     vec3 col = march(from, dir);
// 	col*=vec3(1.,.9,.8);
//     out_Pixel = vec4(col,1.0);
// }