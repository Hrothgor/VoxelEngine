#version 430 core

in vec2 uv;

uniform float iTime;
uniform vec2 iResolution;
uniform mat4 iViewMatrix;

uniform sampler2D iAlbedo;
uniform sampler2D iNormal;
uniform sampler2D iPosition;
uniform sampler2D iDepth;

out vec4 FragColor;

void main()
{
    vec3 ViewPos = iViewMatrix[3].xyz;

    vec3 Albedo = texture(iAlbedo, uv).rgb;
    vec3 Normal = texture(iNormal, uv).rgb;
    vec3 Position = texture(iPosition, uv).rgb;
    float Depth = texture(iDepth, uv).r;

    vec3 SunPos = vec3(512.0);
    // Rotate in X Z around Y
    SunPos.x = 512.0 * cos(iTime);
    SunPos.z = 512.0 * sin(iTime);
    vec3 SunColor = vec3(1.0, 1.0, 0.54);

    // then calculate lighting as usual
    vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(ViewPos - Position);
    // for(int i = 0; i < NR_LIGHTS; ++i)
    // {
        // diffuse
        vec3 lightDir = normalize(SunPos - Position);
        vec3 diffuse = max(dot(Normal, lightDir), 0.2) * Albedo * (0.3*SunColor);
        lighting += (diffuse * 4);
    // }
    
    FragColor = vec4(lighting, 1.0);
}

// void main()
// {
//     vec2 fragCoord = uv * iResolution;
    
//     float gridThickness = 5.0;
//     // Draw border around textures
//     if (fragCoord.x < gridThickness || fragCoord.x > iResolution.x - gridThickness || fragCoord.y < gridThickness || fragCoord.y > iResolution.y - gridThickness
//         || (fragCoord.x > iResolution.x / 2 - (gridThickness / 2) && fragCoord.x < iResolution.x / 2 + (gridThickness / 2))
//         || (fragCoord.y > iResolution.y / 2 - (gridThickness / 2) && fragCoord.y < iResolution.y / 2 + (gridThickness / 2)))
//     {
//         FragColor = vec4(1.0, 1.0, 1.0, 1.0);
//         return;
//     }

//     // Display albedo then normal then position then depth in every corner of the screen
//     if (fragCoord.x < iResolution.x / 2 && fragCoord.y < iResolution.y / 2)
//     {
//         FragColor = vec4(texture(iAlbedo, vec2(uv.x * 2, uv.y * 2)).rgb, 1.0);
//     }
//     else if (fragCoord.x > iResolution.x / 2 && fragCoord.y < iResolution.y / 2)
//     {
//         FragColor = vec4(texture(iNormal, vec2((uv.x - 0.5) * 2, uv.y * 2)).rgb, 1.0);
//     }
//     else if (fragCoord.x < iResolution.x / 2 && fragCoord.y > iResolution.y / 2)
//     {
//         FragColor = vec4(texture(iPosition, vec2(uv.x * 2, (uv.y - 0.5) * 2)).rgb / 255.0, 1.0);
//     }
//     else if (fragCoord.x > iResolution.x / 2 && fragCoord.y > iResolution.y / 2)
//     {
//         FragColor = vec4(vec3(texture(iDepth, vec2((uv.x - 0.5) * 2, (uv.y - 0.5) * 2)).r), 1.0);
//     }
// }