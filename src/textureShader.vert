/**
 * @file litShader.vert
 *
 * @brief Vertex shader for a material that is assigned a texture.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#version 400 core

uniform mat4 mvMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

in vec4 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec2 vUV;

out vec2 fUV;
out vec3 fNormal;
out vec3 fTangent;
out vec3 fBitangent;
out vec3 fPosition;

void main()
{
	vec4 vEyeCoord = mvMatrix * vPosition;
	gl_Position = projMatrix * vEyeCoord;

	fPosition = vEyeCoord.xyz;

	fNormal = normalize(normalMatrix * vNormal);
	fTangent = normalize(mat3(mvMatrix) * vTangent); 

	fTangent = normalize(fTangent - dot(fTangent, fNormal) * fNormal);

	fBitangent = cross(fNormal, fTangent);

	fUV = vUV;
}
