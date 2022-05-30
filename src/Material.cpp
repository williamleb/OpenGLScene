/**
 * @file Material.cpp
 *
 * @brief ShaderProgram wrapper that allows easy parametrization.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "Material.h"

Material::Material()
{
	m_shaderProgram = std::make_unique<ShaderProgram>();
}

bool Material::init()
{
	bool shaderSuccess = true;
	shaderSuccess &= initShaders();
	shaderSuccess &= m_shaderProgram->link();
	if (!shaderSuccess) {
		std::cerr << "Error when loading main shader" << std::endl;
		return false;
	}

	if (!init_impl()) return false;

	return true;
}

void Material::bind() const
{
	m_shaderProgram->bind();
}

bool Material::initShaders()
{
	bool shaderSuccess = true;
	shaderSuccess &= m_shaderProgram->addShaderFromSource(GL_VERTEX_SHADER, directory + vertexShader());
	shaderSuccess &= m_shaderProgram->addShaderFromSource(GL_FRAGMENT_SHADER, directory + fragmentShader());
	if (!tessControlShader().empty()) shaderSuccess &= m_shaderProgram->addShaderFromSource(GL_TESS_CONTROL_SHADER, directory + tessControlShader());
	if (!tessEvaluationShader().empty()) shaderSuccess &= m_shaderProgram->addShaderFromSource(GL_TESS_EVALUATION_SHADER, directory + tessEvaluationShader());
	if (!geometryShader().empty()) shaderSuccess &= m_shaderProgram->addShaderFromSource(GL_GEOMETRY_SHADER, directory + geometryShader());
	if (!computeShader().empty()) shaderSuccess &= m_shaderProgram->addShaderFromSource(GL_COMPUTE_SHADER, directory + computeShader());

	return shaderSuccess;
}

void Material::setProjectionMatrix(const glm::mat4& projection) const
{
	m_shaderProgram->setMat4(projectionAttributeName, projection);
}

void Material::setModelViewMatrix(const glm::mat4& modelView) const
{
	m_shaderProgram->setMat4(modelViewAttributeName, modelView);
}

void Material::setNormalMatrix(const glm::mat3& normalMatrix) const
{
	m_shaderProgram->setMat3(normalMatrixAttributeName, normalMatrix);
}

void Material::setViewMatrix(const glm::mat4& viewMatrix) const
{
	m_shaderProgram->setMat4(viewMatrixAttributeName, viewMatrix);
}
