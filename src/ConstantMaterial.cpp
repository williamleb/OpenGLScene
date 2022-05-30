/**
 * @file ConstantMaterial.cpp
 *
 * @brief Material that doesn't lighting into account.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "ConstantMaterial.h"

#include <glad/glad.h>
#include <iostream>

ConstantMaterial::ConstantMaterial() : Material()
{
}

void ConstantMaterial::bind() const
{
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Material::bind();
}

GLint ConstantMaterial::positionAttribLocation() const
{
	return m_vPositionLocation;
}

GLint ConstantMaterial::normalAttribLocation() const
{
	return GLint(-1);
}

GLint ConstantMaterial::tangentAttribLocation() const
{
	return GLint(-1);
}

GLint ConstantMaterial::uvAttribLocation() const
{
	return GLint(-1);
}

void ConstantMaterial::setAppearance(const glm::vec4& ambiantColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, float specularTerm) const
{
	setColor(diffuseColor);
}

void ConstantMaterial::setColor(const glm::vec4& color) const
{
	m_shaderProgram->setVec4(uColorAttributeName, color);
}

void ConstantMaterial::setWireframe(bool value)
{
	m_wireframe = value;
}

bool ConstantMaterial::init_impl()
{
	if ((m_vPositionLocation = m_shaderProgram->attributeLocation(vPositionAttributeName)) < 0) {
		std::cerr << "Unable to find shader location for " << vPositionAttributeName << std::endl;
		return false;
	}

	return true;
}

