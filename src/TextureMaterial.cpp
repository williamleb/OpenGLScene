/**
 * @file TextureMaterial.cpp
 *
 * @brief Material that takes lighting into account.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "TextureMaterial.h"

#include <glad/glad.h>
#include <iostream>

void TextureMaterial::bind() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	Material::bind();
}


GLint TextureMaterial::positionAttribLocation() const
{
	return m_vPositionLocation;
}

GLint TextureMaterial::normalAttribLocation() const
{
	return m_vNormalLocation;
}

GLint TextureMaterial::tangentAttribLocation() const
{
	return m_vTangentLocation;
}

GLint TextureMaterial::uvAttribLocation() const
{
	return m_vUVLocation;
}

void TextureMaterial::setLightColor(const glm::vec3& color) const
{
	m_shaderProgram->setVec3(uLightColorAttributeName, color);
}

void TextureMaterial::setLightIntensity(float intensity) const
{
    m_shaderProgram->setFloat(uLightIntensityAttributeName, intensity);
}

void TextureMaterial::setDirectionalLightOrientation(const glm::vec3& ori) const
{
    m_shaderProgram->setVec3(uDirectionalLightOrientationAttributeName, ori);
}

void TextureMaterial::setDirectionalLightIntensity(float intensity) const
{
    m_shaderProgram->setFloat(uDirectionalLightIntensityAttributeName, intensity);
}

void TextureMaterial::setSpecular(const float specular) const
{
    m_shaderProgram->setFloat(uSpecularAttributeName, specular);
}

void TextureMaterial::setAppearance(const glm::vec4& ambiantColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, float specularTerm) const
{
	m_shaderProgram->setVec4(uKaAttributeName, ambiantColor);
	m_shaderProgram->setVec4(uKdAttributeName, diffuseColor);
	m_shaderProgram->setVec4(uKsAttributeName, specularColor);
	m_shaderProgram->setFloat(uKnAttributeName, specularTerm);
}

void TextureMaterial::setTexture(unsigned int textureId) const
{
	glActiveTexture(texUnitValue);
	glBindTexture(GL_TEXTURE_2D, textureId);
	setTextureLocation();
}

void TextureMaterial::setNormalsTexture(unsigned int textureId) const
{
	glActiveTexture(normalsTexUnitValue);
	glBindTexture(GL_TEXTURE_2D, textureId);
	setNormalsTextureLocation();
}

bool TextureMaterial::init_impl()
{
	if ((m_vPositionLocation = m_shaderProgram->attributeLocation(vPositionAttributeName)) < 0) {
		std::cerr << "Unable to find shader location for " << vPositionAttributeName << std::endl;
		return false;
	}

	if ((m_vNormalLocation = m_shaderProgram->attributeLocation(vNormalAttributeName)) < 0) {
		std::cerr << "Unable to find shader location for " << vNormalAttributeName << std::endl;
		return false;
	}

	if ((m_vTangentLocation = m_shaderProgram->attributeLocation(vTangentAttributeName)) < 0) {
		std::cerr << "Unable to find shader location for " << vTangentAttributeName << std::endl;
		return false;
	}

	if ((m_vUVLocation = m_shaderProgram->attributeLocation(vUVAttributeName)) < 0) {
		std::cerr << "Unable to find shader location for " << vUVAttributeName << std::endl;
		return false;
	}

	if (!setTextureLocation()) {
		std::cerr << "Unable to find shader location for " << uTexAttributeName << "\n";
		//return false;
	}

	if (!setNormalsTextureLocation()) {
		std::cerr << "Unable to find shader location for " << uNormalsTexAttributeName << "\n";
		//return false;
	}

	return true;
}

bool TextureMaterial::setTextureLocation() const
{
	const auto textureLocation = m_shaderProgram->uniformAttributeLocation(uTexAttributeName);
	if (textureLocation < 0) {
		return false;
	}
	glUniform1i(textureLocation, texUnit);
	return true;
}

bool TextureMaterial::setNormalsTextureLocation() const
{
	const int normalsTextureLoc = m_shaderProgram->uniformAttributeLocation(uNormalsTexAttributeName);
	if (normalsTextureLoc < 0) {
		return false;
	}
	glUniform1i(normalsTextureLoc, normalsTexUnit);
	return true;
}

