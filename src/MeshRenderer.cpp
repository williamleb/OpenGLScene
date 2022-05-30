/**
 * @file MeshRenderer.cpp
 *
 * @brief A scene object which is used to draw a mesh.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "MeshRenderer.h"

#include <cassert>
#include <glm/gtc/matrix_inverse.hpp>

#include "Material.h"
#include "ConstantMaterial.h"
#include "Camera.h"
#include "Mesh.h"
#include "OBJLoader.h"

inline glm::uvec4 getRGBA(uint32_t packedUint) {
	const unsigned int blue = packedUint & 255;
	const unsigned int green = (packedUint >> 8) & 255;
	const unsigned int red = (packedUint >> 16) & 255;
	const unsigned int alpha = (packedUint >> 24) & 255;

	return glm::uvec4(red, green, blue, alpha);
}

MeshRenderer::MeshRenderer(const std::shared_ptr<const Mesh>& mesh, const std::shared_ptr<const Material>& material, const std::shared_ptr<const ConstantMaterial>& constantMaterial)
	: SceneObject(), m_mesh(mesh), m_material(material), m_constantMaterial(constantMaterial)
{
	assert(mesh != nullptr);
	assert(material != nullptr);
	assert(constantMaterial != nullptr);
}

MeshRenderer::MeshRenderer(SceneObject& parent, const std::shared_ptr<const Mesh>& mesh, const std::shared_ptr<const Material>& material, const std::shared_ptr<const ConstantMaterial>& constantMaterial)
	: SceneObject(parent), m_mesh(mesh), m_material(material), m_constantMaterial(constantMaterial)
{
	assert(mesh != nullptr);
	assert(material != nullptr);
	assert(constantMaterial != nullptr);
}

void MeshRenderer::getFace(const glm::vec3& worldPosition, glm::vec3& outLocalCenter, glm::vec3& outLocalNormal) const
{
	const glm::vec3 localPosition = glm::inverse(modelMatrix())* glm::vec4(worldPosition, 1.0f);
	m_mesh->faceAt(localPosition, outLocalCenter, outLocalNormal);
}

void MeshRenderer::setColorsFromObjectLoader(OBJLoader::Loader loader, unsigned materialId)
{
	assert(("Loader must be loaded", loader.isLoaded()));
	assert(("Invalid material Id", loader.getMaterials().size() > materialId));

	setColors(loader.getMaterials()[materialId]);
}

void MeshRenderer::setColors(OBJLoader::Material materialData)
{
	ambiantColor(glm::vec4(materialData.Ka[0], materialData.Ka[1], materialData.Ka[2], materialData.Ka[3]));
	diffuseColor(glm::vec4(materialData.Kd[0], materialData.Kd[1], materialData.Kd[2], materialData.Kd[3]));
	specularColor(glm::vec4(materialData.Ks[0], materialData.Ks[1], materialData.Ks[2], materialData.Ks[3]));
	specularTerm(materialData.Kn);
}

void MeshRenderer::renderImplementation(const Camera& camera, const glm::mat4& modelMatrix)
{
	if (selected())
	{
		bindAndUpdateMaterialMatrices(*m_constantMaterial, camera, modelMatrix);
		m_constantMaterial->setColor(m_selectedColor);
		m_mesh->bindAndDrawConstant();
	}
	else
	{
		m_material->setAppearance(m_ambiantColor, m_diffuseColor, m_specularColor, m_specularTerm);
		m_material->setTexture(m_textureIndex);
		m_material->setNormalsTexture(m_normalsTextureIndex);

		bindAndUpdateMaterialMatrices(*m_material, camera, modelMatrix);
		m_mesh->bindAndDraw();
	}
}

void MeshRenderer::renderIdImplementation(const Camera& camera, const glm::mat4& modelMatrix)
{
	bindAndUpdateMaterialMatrices(*m_constantMaterial, camera, modelMatrix);

	const auto color = getRGBA(id());
	const auto colorFloat = glm::vec4(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
	m_constantMaterial->setColor(colorFloat);

	m_mesh->bindAndDrawConstant();
}

void MeshRenderer::bindAndUpdateMaterialMatrices(const Material& material, const Camera& camera, const glm::mat4& modelMatrix) const
{
	material.bind();

	const glm::mat4 viewMatrix = camera.viewMatrix();
	const glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	const glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(modelViewMatrix));

	material.setModelViewMatrix(modelViewMatrix);
    material.setViewMatrix(viewMatrix);
	material.setProjectionMatrix(camera.projectionMatrix());
	material.setNormalMatrix(normalMat);
}
