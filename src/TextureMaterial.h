#pragma once
#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

/**
 * @file TextureMaterial.h
 *
 * @brief Material that takes lighting into account.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "Material.h"

class TextureMaterial : public Material {
public:

	void bind() const override;
	GLint positionAttribLocation() const override;
	GLint normalAttribLocation() const override;
	GLint tangentAttribLocation() const override;
	GLint uvAttribLocation() const override;

	void setLightColor(const glm::vec3& color) const;
	void setLightIntensity(float) const;
    void setDirectionalLightOrientation(const glm::vec3& color) const;
	void setDirectionalLightIntensity(float) const;
	void setSpecular(float specular) const;

	void setAppearance(const glm::vec4& ambiantColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, float specularTerm) const override;
	void setTexture(unsigned int textureId) const override;
	void setNormalsTexture(unsigned int textureId) const override;

protected:
	bool init_impl() override;

	inline std::string vertexShader() const override { return "textureShader.vert"; }
	inline std::string fragmentShader() const override { return "textureShader.frag"; }

private:
	bool setTextureLocation() const;
	bool setNormalsTextureLocation() const;

    bool isSetup = false;

	int m_vPositionLocation = -1;
	int m_vNormalLocation = -1;
	int m_vTangentLocation = -1;
	int m_vUVLocation = -1;

	const std::string vPositionAttributeName = "vPosition";
	const std::string vNormalAttributeName = "vNormal";
	const std::string vTangentAttributeName = "vTangent";
	const std::string vUVAttributeName = "vUV";

	const std::string uLightColorAttributeName = "uLightColor";
	const std::string uLightIntensityAttributeName = "uLightIntensity";
    const std::string uDirectionalLightOrientationAttributeName = "uDLightOri";
    const std::string uDirectionalLightIntensityAttributeName = "uDLightIntensity";
	const std::string uSpecularAttributeName = "uSpecular";

	const std::string uTexAttributeName = "uTex";
	const std::string uNormalsTexAttributeName = "uNormalsTex";

	const std::string uKaAttributeName = "uKa";
	const std::string uKdAttributeName = "uKd";
	const std::string uKsAttributeName = "uKs";
	const std::string uKnAttributeName = "uKn";

	const int texUnit = 0;
	const int normalsTexUnit = 1;

	const int texUnitValue = GL_TEXTURE0;
	const int normalsTexUnitValue = GL_TEXTURE1;
};
#endif
