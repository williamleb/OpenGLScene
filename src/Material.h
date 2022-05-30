#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

/**
 * @file Material.h
 *
 * @brief ShaderProgram wrapper that allows easy parametrization.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <memory>
#include <string>

#include "ShaderProgram.h"

class Material {
public:
	virtual ~Material() = default;
	Material();

	bool init();
	virtual void bind() const;

	virtual GLint positionAttribLocation() const = 0;
	virtual GLint normalAttribLocation() const = 0;
	virtual GLint tangentAttribLocation() const = 0;
	virtual GLint uvAttribLocation() const = 0;

	void setProjectionMatrix(const glm::mat4& projection) const;
	void setModelViewMatrix(const glm::mat4& modelView) const;
	void setNormalMatrix(const glm::mat3& normalMatrix) const;
	void setViewMatrix(const glm::mat4& viewMatrix) const;

	virtual void setAppearance(const glm::vec4& ambiantColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, float specularTerm) const {}
	virtual void setTexture(unsigned int textureId) const {}
	virtual void setNormalsTexture(unsigned int textureId) const {}

protected:
	virtual bool init_impl() { return true; };

	virtual inline std::string vertexShader() const = 0;
	virtual inline std::string fragmentShader() const = 0;
	virtual inline std::string tessControlShader() const { return ""; }
	virtual inline std::string tessEvaluationShader() const { return ""; }
	virtual inline std::string geometryShader() const { return ""; }
	virtual inline std::string computeShader() const { return ""; }

private:
	bool initShaders();

protected:
	std::unique_ptr<ShaderProgram> m_shaderProgram = nullptr;

private:
	const std::string directory = SHADERS_DIR;

	const std::string projectionAttributeName = "projMatrix";
	const std::string modelViewAttributeName = "mvMatrix";
	const std::string normalMatrixAttributeName = "normalMatrix";
	const std::string viewMatrixAttributeName = "viewMatrix";

};
#endif
