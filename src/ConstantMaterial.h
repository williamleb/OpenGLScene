#pragma once
#ifndef BASICMATERIAL_H
#define CONSTANTMATERIAL_H

/**
 * @file ConstantMaterial.h
 *
 * @brief Material that doesn't lighting into account.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "Material.h"

class ConstantMaterial : public Material {
public:
	ConstantMaterial();

	virtual void bind() const override;
	virtual GLint positionAttribLocation() const override;
	virtual GLint normalAttribLocation() const override;
	virtual GLint tangentAttribLocation() const override;
	virtual GLint uvAttribLocation() const override;

	void setAppearance(const glm::vec4& ambiantColor, const glm::vec4& diffuseColor, const glm::vec4& specularColor, float specularTerm) const override;
	void setColor(const glm::vec4& color) const;
	void setWireframe(bool value);

protected:
	virtual bool init_impl() override;

	virtual inline std::string vertexShader() const override { return "constantShader.vert"; }
	virtual inline std::string fragmentShader() const override { return "constantShader.frag"; }

private:
	int m_vPositionLocation = -1;

	const std::string vPositionAttributeName = "vPosition";
	const std::string uColorAttributeName = "uColor";

	bool m_wireframe = false;
};
#endif
