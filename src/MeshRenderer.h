#pragma once
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

/**
 * @file MeshRenderer.h
 *
 * @brief A scene object which is used to draw a mesh.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <memory>

#include "SceneObject.h"

class Material;
class ConstantMaterial;
class Mesh;

namespace OBJLoader
{
	struct Material;
	class Loader;
}

class MeshRenderer : public SceneObject
{
public:
	MeshRenderer(const std::shared_ptr<const Mesh>& mesh, const std::shared_ptr<const Material>& material, const std::shared_ptr<const ConstantMaterial>& constantMaterial);
	MeshRenderer(SceneObject& parent, const std::shared_ptr<const Mesh>& mesh, const std::shared_ptr<const Material>& material, const std::shared_ptr<const ConstantMaterial>& constantMaterial);

	void getFace(const glm::vec3& worldPosition, glm::vec3& outLocalCenter, glm::vec3& outLocalNormal) const;

	inline void selectedColor(const glm::vec4& newSelectedColor) { m_selectedColor = newSelectedColor; }
	inline void textureIndex(unsigned int newTextureIndex) { m_textureIndex = newTextureIndex; }
	inline void normalsTextureIndex(unsigned int newNormalsTextureIndex) { m_normalsTextureIndex = newNormalsTextureIndex; }

	inline glm::vec4& ambiantColor() { return m_ambiantColor; }
	inline glm::vec4& diffuseColor() { return m_diffuseColor; }
	inline glm::vec4& specularColor() { return m_specularColor; }
	inline float& specularTerm() { return m_specularTerm; }

	inline const glm::vec4& ambiantColor() const { return m_ambiantColor; }
	inline const glm::vec4& diffuseColor() const { return m_diffuseColor; }
	inline const glm::vec4& specularColor() const { return m_specularColor; }
	inline const float& specularTerm() const { return m_specularTerm; }

	inline void ambiantColor(const glm::vec4& newAmbiantColor) { m_ambiantColor = newAmbiantColor; }
	inline void diffuseColor(const glm::vec4& newDiffuseColor) { m_diffuseColor = newDiffuseColor; }
	inline void specularColor(const glm::vec4& newSpecularColor) { m_specularColor = newSpecularColor; }
	inline void specularTerm(float newSpecularTerm) { m_specularTerm = newSpecularTerm; }

	void setColorsFromObjectLoader(OBJLoader::Loader loader, unsigned int materialId);

protected:
	virtual void renderImplementation(const Camera& camera, const glm::mat4& modelMatrix) override;
	virtual void renderIdImplementation(const Camera& camera, const glm::mat4& modelMatrix) override;

	void bindAndUpdateMaterialMatrices(const Material& material, const Camera& camera, const glm::mat4& modelMatrix) const;

private:
	void setColors(OBJLoader::Material materialData);

	std::shared_ptr<const Mesh> m_mesh;
	std::shared_ptr<const Material> m_material;
	std::shared_ptr<const ConstantMaterial> m_constantMaterial;

	glm::vec4 m_selectedColor = glm::vec4(1, 0, 0, 1);

	int m_textureIndex = 1;
	int m_normalsTextureIndex = 2;

	glm::vec4 m_ambiantColor = glm::vec4(0.05f);
	glm::vec4 m_diffuseColor = glm::vec4(1.0f);
	glm::vec4 m_specularColor = glm::vec4(1.0f);
	float m_specularTerm = 128.0f;
};

#endif
