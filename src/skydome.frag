#version 400 core
uniform sampler2D skydomeTex1;
uniform sampler2D skydomeTex2;

uniform vec3 sunPos;
uniform vec2 sunRot;
uniform vec3 viewDir;

in vec3 fPosition;

out vec4 oColor;

#define PI 3.14159265358979323844

void
main()
{


    vec3 normPos = normalize(fPosition);
    vec3 nSun = normalize(sunPos);

    if (distance(normPos,nSun) < 0.05){
        oColor = vec4(1.0);
    }
    else {

        // UV: coordonné de texture en fonction de l'élévation du soleil et de l'élévation du point de vue
        // orientation: produit scalaire entre la direction de vue et le soleil
        float phi = atan(normPos.z, normPos.x) * 0.5 / PI + 0.5;
        float theta = 1.0 - acos(normPos.y) / PI;
        vec2 uv = vec2(viewDir.y, sunRot.y);

        float orientation = dot(uv, sunRot);

        vec4 color1 = texture(skydomeTex1,uv.xy);
        vec4 color2 = texture(skydomeTex2,uv.xy);

        oColor = mix(color2, color1, orientation * 0.5 + 0.5);
    }
}