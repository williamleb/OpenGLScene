#pragma once
#ifndef OBJECTMESH_H
#define OBJECTMESH_H

/**
 * @file ObjectMesh.h
 *
 * @brief Mesh that takes the shape of a mesh described in an object.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Mesh.h"

namespace OBJLoader
{
	struct Mesh;
	struct Vertex;
}

class ObjectMesh : public Mesh
{
public:
	void init(const OBJLoader::Mesh& objectMesh);
	void initAttributes(const std::shared_ptr<const Material>& material) const override;
	void initConstantAttributes(const std::shared_ptr<const Material>& constantMaterial) const override;

	const std::vector<GLfloat>& vertices() override { return m_vertices; }
	const std::vector<GLfloat>& normals() override { return m_normals; }
	const std::vector<GLfloat>& tangents() override { return m_tangents; }
	const std::vector<GLfloat>& uvs() override { return m_uvs; }
	const std::vector<GLuint>& indices() override { return m_indices; }

	void faceAt(const glm::vec3& position, glm::vec3& outCenter, glm::vec3& outNormal) const override;

	void bindAndDraw() const override;
	void bindAndDrawConstant() const override;

private:
	void init() override;

	void initVertices(const OBJLoader::Mesh& objectMesh);
	void initNormals(const OBJLoader::Mesh& objectMesh);
	void initTangents(const OBJLoader::Mesh& objectMesh);
	void initIndices(const OBJLoader::Mesh& objectMesh);
	void initUVs(const OBJLoader::Mesh& objectMesh);
	void initBuffersAndVAOs();
	void initCubeBuffersAndVAOs();
	void initCubeConstantBuffersAndVAOs();

private:
	std::vector<GLfloat> m_vertices;
	std::vector<GLfloat> m_normals;
	std::vector<GLfloat> m_tangents;
	std::vector<GLuint> m_indices;
	std::vector<GLfloat> m_uvs;

	enum VAO_IDs { VAO_Object, VAO_ObjectConstant, NumVAOs };
	enum Buffer_IDs { VBO_Object, EBO_Object, VBO_ObjectConstant, EBO_ObjectConstant, NumBuffers };

	GLuint m_VAOs[NumVAOs];
	GLuint m_buffers[NumBuffers];
};

#endif
