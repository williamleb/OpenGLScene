/**
 * @file constantShader.vert
 *
 * @brief Vertex shader for a material that isn't affected by light.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#version 400 core
uniform mat4 mvMatrix;
uniform mat4 projMatrix;

in vec4 vPosition;

void main()
{
  gl_Position = projMatrix * mvMatrix * vPosition;
}

