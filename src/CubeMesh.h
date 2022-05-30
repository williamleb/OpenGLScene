#pragma once
#ifndef CUBEMESH_H
#define CUBEMESH_H

/**
 * @file CubeMesh.h
 *
 * @brief Mesh that takes the shape of a cube.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Mesh.h"

class CubeMesh : public Mesh
{
public:
	void init() override;
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
	void initVertices();
	void initNormals();
	void initTangents();
	void initIndices();
	void initUVs();
	void initBuffersAndVAOs();
	void initCubeBuffersAndVAOs();
	void initCubeConstantBuffersAndVAOs();

	template <class T>
	static void pushBackVector(std::vector<T>& arrayToPushBackTo, glm::vec3& vector)
	{
		arrayToPushBackTo.push_back(vector.x);
		arrayToPushBackTo.push_back(vector.y);
		arrayToPushBackTo.push_back(vector.z);
	}

	template <class T>
	static void pushBackVector(std::vector<T>& arrayToPushBackTo, glm::vec2& vector)
	{
		arrayToPushBackTo.push_back(vector.x);
		arrayToPushBackTo.push_back(vector.y);
	}

private:
	std::vector<GLfloat> m_vertices;
	std::vector<GLfloat> m_normals;
	std::vector<GLfloat> m_tangents;
	std::vector<GLuint> m_indices;
	std::vector<GLfloat> m_uvs;

	enum VAO_IDs { VAO_Cube, VAO_CubeConstant, NumVAOs };
	enum Buffer_IDs { VBO_Cube, EBO_Cube, VBO_CubeConstant, EBO_CubeConstant, NumBuffers };

	GLuint m_VAOs[NumVAOs];
	GLuint m_buffers[NumBuffers];
};

#endif
