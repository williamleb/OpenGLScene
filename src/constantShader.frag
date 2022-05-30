/**
 * @file constantShader.frag
 *
 * @brief Fragment shader for a material that isn't affected by light.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#version 400 core

uniform vec4 uColor;

out vec4 fColor;

void main()
{
  fColor = uColor;
}
