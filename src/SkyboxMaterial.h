#pragma once
#ifndef SKYBOXMATERIAL_H
#define SKYBOXMATERIAL_H

/**
 * @file SkyBoxMaterial.h
 *
 * @brief Material that takes lighting into account.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "Material.h"
#include "MainWindow.h"

class SkyboxMaterial : public Material {
public:

	void bind() const override;
	[[nodiscard]] GLint positionAttribLocation() const override;
	[[nodiscard]] GLint normalAttribLocation() const override;
    [[nodiscard]] GLint tangentAttribLocation() const override;
    [[nodiscard]] GLint uvAttribLocation() const override;

	void setProjMatrix(const glm::mat4& color) const;
	void setViewRotMatrix(const glm::mat3& color) const;
    void setSunPos(const glm::vec3& sunPos) const;
    void setSunRot(const glm::vec2& sunRot) const;
    void setViewDir(const glm::vec3& viewDir) const;

    void bindTextureAndDraw() const;

protected:
	bool init_impl() override;

	[[nodiscard]] inline std::string vertexShader() const override { return "skydome.vert"; }
	[[nodiscard]] inline std::string fragmentShader() const override { return "skydome.frag"; }

private:

    void loadTexture(unsigned int& textureId, const std::string& fileName);
    void initGeometrySphere();

	int m_vPositionLocation = -1;
	int m_vNormalLocation = -1;
    int m_vUVLocation = -1;

    unsigned int TextureIdForward = -1;
    unsigned int TextureIdBackward = -1;

	const std::string vPositionAttributeName = "vPosition";
	const std::string viewRotMatrixAttributeName = "viewRotMatrix";
	const std::string projMatrixAttributeName = "projMatrix";
	const std::string sunPosAttributeName = "sunPos";
	const std::string sunRotAttributeName = "sunRot";
	const std::string viewDirAttributeName = "viewDir";
	const std::string skyDomeTex1AttributeName = "skydomeTex1";
    const std::string skyDomeTex2AttributeName = "skydomeTex2";

    enum VAO_IDs { VAO_Sphere_Sky, VAO_Sphere, NumVAOs };
    enum Buffer_IDs { VBO_Sphere_Position, VBO_Sphere_Normal, EBO_Sphere, NumBuffers };

    // Sphere
    int numRowSphere = 20;
    int numColSphere = numRowSphere + 2;
    int numTriSphere = numColSphere * (numRowSphere - 1) * 2 + 2 * numColSphere;

    GLuint m_VAOs[NumVAOs];
    GLuint m_buffers[NumBuffers];

};
#endif
