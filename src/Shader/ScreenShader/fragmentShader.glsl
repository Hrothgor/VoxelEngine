#version 430 core
in vec2 uv;

uniform vec2 iResolution;

uniform sampler2D iAlbedo;
uniform sampler2D iNormal;
uniform sampler2D iPosition;
uniform sampler2D iDepth;

out vec4 fragColor;

vec3 MultiSampleFromTexture(sampler2D tex, vec2 uv, vec2 resolution, int samples)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < samples; i++)
    {
        vec2 offset = vec2(float(i % 2) / resolution.x, float(i / 2) / resolution.y);
        result += texture(tex, uv + offset).rgb;
    }
    return result / float(samples);
}

// void main()
// {
//     vec2 fragCoord = uv * iResolution;

//     // Display iAlbedo with his 2 mipamp level, it size is 512*512
//     if (fragCoord.x < 512 && fragCoord.y < 512)
//     {
//         fragColor = vec4(vec3(texelFetch(iAlbedo, ivec2(fragCoord), 0).r * 255.0), 1.0);
//         return;
//     }
//     if (fragCoord.x < 512 + 256 && fragCoord.y < 512 + 256 && fragCoord.x > 512 && fragCoord.y > 512)
//     {
//         fragColor = vec4(vec3(texelFetch(iAlbedo, ivec2(fragCoord - vec2(512, 512)), 1).r * 255.0), 1.0);
//         return;
//     }
//     if (fragCoord.x < 512 + 256 + 128 && fragCoord.y < 512 + 256 + 128 && fragCoord.x > 512 + 256 && fragCoord.y > 512 + 256)
//     {
//         fragColor = vec4(vec3(texelFetch(iAlbedo, ivec2(fragCoord - vec2(512 + 256, 512 + 256)), 2).r * 255.0), 1.0);
//         return;
//     }
//     fragColor = vec4(0.0, 1.0, 0.08, 0.2);
// }

void main()
{
    vec2 fragCoord = uv * iResolution;
    
    float gridThickness = 5.0;
    // Draw border around textures
    if (fragCoord.x < gridThickness || fragCoord.x > iResolution.x - gridThickness || fragCoord.y < gridThickness || fragCoord.y > iResolution.y - gridThickness
        || (fragCoord.x > iResolution.x / 2 - (gridThickness / 2) && fragCoord.x < iResolution.x / 2 + (gridThickness / 2))
        || (fragCoord.y > iResolution.y / 2 - (gridThickness / 2) && fragCoord.y < iResolution.y / 2 + (gridThickness / 2)))
    {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }

    // Display albedo then normal then position then depth in every corner of the screen
    if (fragCoord.x < iResolution.x / 2 && fragCoord.y < iResolution.y / 2)
    {
        fragColor = vec4(texture(iAlbedo, vec2(uv.x * 2, uv.y * 2)).rgb, 1.0);
    }
    else if (fragCoord.x > iResolution.x / 2 && fragCoord.y < iResolution.y / 2)
    {
        fragColor = vec4(texture(iNormal, vec2((uv.x - 0.5) * 2, uv.y * 2)).rgb, 1.0);
    }
    else if (fragCoord.x < iResolution.x / 2 && fragCoord.y > iResolution.y / 2)
    {
        fragColor = vec4(texture(iPosition, vec2(uv.x * 2, (uv.y - 0.5) * 2)).rgb, 1.0);
    }
    else if (fragCoord.x > iResolution.x / 2 && fragCoord.y > iResolution.y / 2)
    {
        fragColor = vec4(vec3(texture(iDepth, vec2((uv.x - 0.5) * 2, (uv.y - 0.5) * 2)).r), 1.0);
    }
}