/**
 * @file litShader.frag
 *
 * @brief Fragment shader for a material that is assigned a texture.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#version 400 core

uniform mat4 viewMatrix;

uniform float uLightIntensity;
uniform vec3 uLightColor;
uniform float uDLightIntensity;
uniform vec3 uDLightOri;
uniform float uSpecular;

uniform vec4 uKa;
uniform vec4 uKd;
uniform vec4 uKs;
uniform float uKn;

uniform sampler2D uTex;
uniform sampler2D uNormalsTex;

in vec2 fUV;
in vec3 fNormal;
in vec3 fTangent;
in vec3 fBitangent;
in vec3 fPosition;

out vec4 fColor;

float distanceSquared(vec3 left, vec3 right);

void main()
{
	// Normal
    vec3 nNormal = normalize(fNormal);
	if (length(fTangent) > 0.1)
	{
		mat3 tbn = mat3(normalize(fTangent), normalize(fBitangent), normalize(fNormal));
		vec3 normalFromTexture = texture(uNormalsTex, fUV).rgb * 2.0 - vec3(1.0);
		nNormal = normalize(tbn * normalFromTexture);
	}

    vec3 viewDir = normalize(-fPosition);
	vec4 ka = uKa;
    vec3 kd = uKd.rgb * max(0, min((uSpecular * -2) + 2, 1));
    vec3 ks = uKs.rgb * max(0, min(uSpecular * 2, 1));
	float n = uKn;

    //Direction light
    vec4 lightDir = normalize(vec4(-uDLightOri,0.0));
    vec4 dlightDir = normalize(viewMatrix * lightDir);
    float diff = max(dot(nNormal, dlightDir.xyz),0.0);
    vec3 reflectDir = reflect(-dlightDir.xyz,nNormal);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), n);
    vec4 dlResult =  vec4(1.0) * vec4(kd * diff + ks * spec, 1) * uDLightIntensity;

    //Point light
    vec4 lightColor = vec4(uLightColor, 1);

    vec3 lightPosition = vec3(0.0);
    float lightIntensity = (1 / distanceSquared(fPosition, lightPosition)) * uLightIntensity;

    vec3 lightDirection = normalize(lightPosition - fPosition);
    vec3 nViewDirection = normalize(vec3(0.0)-fPosition);

    float diffuse = max(0, dot(nNormal, lightDirection));
    float specular = 0;

    if (diffuse > 0) {
        vec3 reflectedVector = normalize(-lightDirection+2.0*nNormal*dot(nNormal, lightDirection));
        specular = pow(max(0.0, dot(nViewDirection, reflectedVector)), n);
    }

    vec4 plResult =  lightColor * vec4(kd * diffuse + ks * specular, 1) * lightIntensity;

	// Retrive color from the texture
    vec4 texColor = texture(uTex, fUV);

    fColor = (ka + dlResult + plResult) * texColor;
}

float distanceSquared(vec3 left, vec3 right)
{
    vec3 direction = left - right;
    return max(dot(direction, direction), 1);
}
