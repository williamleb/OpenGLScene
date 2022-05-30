#version 400 core

uniform mat3 viewRotMatrix;
uniform mat4 projMatrix;

in vec3 vPosition;

out vec3 fPosition;

void main()
{
     vec3 vEyeCoord = viewRotMatrix * vPosition;
     gl_Position = projMatrix * vec4(vEyeCoord, 1.0);

     fPosition = vPosition.xyz;
}

