/**
 * @file CubeMesh.cpp
 *
 * @brief Mesh that takes the shape of a cube.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "CubeMesh.h"

#include <glm/vec3.hpp>

#include "Material.h"

void CubeMesh::init()
{
	initVertices();
	initNormals();
	initTangents();
	initIndices();
	initUVs();
	initBuffersAndVAOs();
}

void CubeMesh::initAttributes(const std::shared_ptr<const Material>& material) const
{
	glBindVertexArray(m_VAOs[VAO_Cube]);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Cube]);

	const int positionAttributeLocation = material->positionAttribLocation();
	glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(positionAttributeLocation);

	const int uvAttributeLocation = material->uvAttribLocation();
	glVertexAttribPointer(uvAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(static_cast<long>(sizeof(GLfloat) * m_vertices.size())));
	glEnableVertexAttribArray(uvAttributeLocation);

	const int normalAttributeLocation = material->normalAttribLocation();
	glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(static_cast<long>(sizeof(GLfloat) * (m_vertices.size() + m_uvs.size()))));
	glEnableVertexAttribArray(normalAttributeLocation);

	const int tangentAttributeLocation = material->tangentAttribLocation();
	glVertexAttribPointer(tangentAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(static_cast<long>(sizeof(GLfloat) * (m_vertices.size() + m_uvs.size() + m_normals.size()))));
	glEnableVertexAttribArray(tangentAttributeLocation);
}

void CubeMesh::initConstantAttributes(const std::shared_ptr<const Material>& constantMaterial) const
{
	glBindVertexArray(m_VAOs[VAO_CubeConstant]);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_CubeConstant]);

	const int positionLocation = constantMaterial->positionAttribLocation();
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(positionLocation);
}

void CubeMesh::faceAt(const glm::vec3& position, glm::vec3& outCenter, glm::vec3& outNormal) const
{
	if (abs(position.x) > abs(position.y) && abs(position.x) > abs(position.z))
	{
		if (position.x > 0)
		{
			outCenter = glm::vec3(0.5f, 0.0f, 0.0f);
			outNormal = glm::vec3(1, 0, 0);
		}
		else
		{
			outCenter = glm::vec3(-0.5f, 0.0f, 0.0f);
			outNormal = glm::vec3(-1, 0, 0);
		}
	}
	else if (abs(position.y) > abs(position.z))
	{
		if (position.y > 0)
		{
			outCenter = glm::vec3(0.0f, 0.5f, 0.0f);
			outNormal = glm::vec3(0, 1, 0);
		}
		else
		{
			outCenter = glm::vec3(0.0f, -0.5f, 0.0f);
			outNormal = glm::vec3(0, -1, 0);
		}
	}
	else
	{
		if (position.z > 0)
		{
			outCenter = glm::vec3(0.0f, 0.0f, 0.5f);
			outNormal = glm::vec3(0, 0, 1);
		}
		else
		{
			outCenter = glm::vec3(0.0f, 0.0f, -0.5f);
			outNormal = glm::vec3(0, 0, -1);
		}
	}
}

void CubeMesh::bindAndDraw() const
{
	const GLsizei sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());
	glBindVertexArray(m_VAOs[VAO_Cube]);
	glDrawElements(GL_TRIANGLES, sizeOfIndices, GL_UNSIGNED_INT, nullptr);
}

void CubeMesh::bindAndDrawConstant() const
{
	const GLsizei sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());
	glBindVertexArray(m_VAOs[VAO_CubeConstant]);
	glDrawElements(GL_TRIANGLES, sizeOfIndices, GL_UNSIGNED_INT, nullptr);
}

void CubeMesh::initVertices()
{
	glm::vec3 upperTopLeft(0.5f, 0.5f, 0.5f);
	glm::vec3 upperTopRight(-0.5f, 0.5f, 0.5f);
	glm::vec3 upperBottomLeft(0.5f, 0.5f, -0.5f);
	glm::vec3 upperBottomRight(-0.5f, 0.5f, -0.5f);

	glm::vec3 lowerTopLeft(0.5f, -0.5f, 0.5f);
	glm::vec3 lowerTopRight(-0.5f, -0.5f, 0.5f);
	glm::vec3 lowerBottomLeft(0.5f, -0.5f, -0.5f);
	glm::vec3 lowerBottomRight(-0.5f, -0.5f, -0.5f);

	// Top face
	pushBackVector(m_vertices, upperTopLeft);
	pushBackVector(m_vertices, upperTopRight);
	pushBackVector(m_vertices, upperBottomLeft);
	pushBackVector(m_vertices, upperBottomRight);

	// Bottom face
	pushBackVector(m_vertices, lowerTopLeft);
	pushBackVector(m_vertices, lowerTopRight);
	pushBackVector(m_vertices, lowerBottomLeft);
	pushBackVector(m_vertices, lowerBottomRight);

	// Forward face
	pushBackVector(m_vertices, upperBottomRight);
	pushBackVector(m_vertices, upperBottomLeft);
	pushBackVector(m_vertices, lowerBottomRight);
	pushBackVector(m_vertices, lowerBottomLeft);

	// Backward face
	pushBackVector(m_vertices, upperTopLeft);
	pushBackVector(m_vertices, upperTopRight);
	pushBackVector(m_vertices, lowerTopLeft);
	pushBackVector(m_vertices, lowerTopRight);

	// Left face
	pushBackVector(m_vertices, upperTopLeft);
	pushBackVector(m_vertices, upperBottomLeft);
	pushBackVector(m_vertices, lowerTopLeft);
	pushBackVector(m_vertices, lowerBottomLeft);

	// Right face
	pushBackVector(m_vertices, upperBottomRight);
	pushBackVector(m_vertices, upperTopRight);
	pushBackVector(m_vertices, lowerBottomRight);
	pushBackVector(m_vertices, lowerTopRight);
}

void CubeMesh::initNormals()
{
	glm::vec3 up(0, 1, 0);
	glm::vec3 down(0, -1, 0);
	glm::vec3 left(1, 0, 0);
	glm::vec3 right(-1, 0, 0);
	glm::vec3 forward(0, 0, -1);
	glm::vec3 backward(0, 0, 1);

	// Top face
	pushBackVector(m_normals, up);
	pushBackVector(m_normals, up);
	pushBackVector(m_normals, up);
	pushBackVector(m_normals, up);

	// Bottom face
	pushBackVector(m_normals, down);
	pushBackVector(m_normals, down);
	pushBackVector(m_normals, down);
	pushBackVector(m_normals, down);

	// Forward face
	pushBackVector(m_normals, forward);
	pushBackVector(m_normals, forward);
	pushBackVector(m_normals, forward);
	pushBackVector(m_normals, forward);

	// Backward face
	pushBackVector(m_normals, backward);
	pushBackVector(m_normals, backward);
	pushBackVector(m_normals, backward);
	pushBackVector(m_normals, backward);

	// Left face
	pushBackVector(m_normals, left);
	pushBackVector(m_normals, left);
	pushBackVector(m_normals, left);
	pushBackVector(m_normals, left);

	// Right face
	pushBackVector(m_normals, right);
	pushBackVector(m_normals, right);
	pushBackVector(m_normals, right);
	pushBackVector(m_normals, right);
}

void CubeMesh::initTangents()
{
	glm::vec3 up(0, 1, 0);
	glm::vec3 down(0, -1, 0);
	glm::vec3 left(1, 0, 0);
	glm::vec3 right(-1, 0, 0);
	glm::vec3 forward(0, 0, -1);
	glm::vec3 leftbackward(0, 0, 1);

	// Top face
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);

	// Bottom face
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);

	// Forward face
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);

	// Backward face
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);
	pushBackVector(m_tangents, left);

	// Left face
	pushBackVector(m_tangents, forward);
	pushBackVector(m_tangents, forward);
	pushBackVector(m_tangents, forward);
	pushBackVector(m_tangents, forward);

	// Right face
	pushBackVector(m_tangents, forward);
	pushBackVector(m_tangents, forward);
	pushBackVector(m_tangents, forward);
	pushBackVector(m_tangents, forward);
}

void CubeMesh::initIndices()
{
	// Top face
	m_indices.push_back(0);
	m_indices.push_back(2);
	m_indices.push_back(1);
	m_indices.push_back(1);
	m_indices.push_back(2);
	m_indices.push_back(3);

	// Bottom face
	m_indices.push_back(4);
	m_indices.push_back(6);
	m_indices.push_back(5);
	m_indices.push_back(5);
	m_indices.push_back(6);
	m_indices.push_back(7);

	// Forward face
	m_indices.push_back(8);
	m_indices.push_back(10);
	m_indices.push_back(9);
	m_indices.push_back(9);
	m_indices.push_back(10);
	m_indices.push_back(11);

	// Backward face
	m_indices.push_back(12);
	m_indices.push_back(14);
	m_indices.push_back(13);
	m_indices.push_back(13);
	m_indices.push_back(14);
	m_indices.push_back(15);

	// Left face
	m_indices.push_back(16);
	m_indices.push_back(18);
	m_indices.push_back(17);
	m_indices.push_back(17);
	m_indices.push_back(18);
	m_indices.push_back(19);

	// Right face
	m_indices.push_back(20);
	m_indices.push_back(22);
	m_indices.push_back(21);
	m_indices.push_back(21);
	m_indices.push_back(22);
	m_indices.push_back(23);
}

void CubeMesh::initUVs()
{
	// Values are not exactly 0s and 1s in order to prevent weird black borders
	glm::vec2 bottomLeft(0.1f, 0.1f);
	glm::vec2 bottomRight(0.9f, 0.1f);
	glm::vec2 topLeft(0.1f, 0.9f);
	glm::vec2 topRight(0.9f, 0.9f);

	// Top face
	pushBackVector(m_uvs, topLeft);
	pushBackVector(m_uvs, topRight);
	pushBackVector(m_uvs, bottomLeft);
	pushBackVector(m_uvs, bottomRight);

	// Bottom face
	pushBackVector(m_uvs, topLeft);
	pushBackVector(m_uvs, topRight);
	pushBackVector(m_uvs, bottomLeft);
	pushBackVector(m_uvs, bottomRight);

	// Forward face
	pushBackVector(m_uvs, topLeft);
	pushBackVector(m_uvs, topRight);
	pushBackVector(m_uvs, bottomLeft);
	pushBackVector(m_uvs, bottomRight);

	// Backward face
	pushBackVector(m_uvs, topLeft);
	pushBackVector(m_uvs, topRight);
	pushBackVector(m_uvs, bottomLeft);
	pushBackVector(m_uvs, bottomRight);

	// Left face
	pushBackVector(m_uvs, topLeft);
	pushBackVector(m_uvs, topRight);
	pushBackVector(m_uvs, bottomLeft);
	pushBackVector(m_uvs, bottomRight);

	// Right face
	pushBackVector(m_uvs, topLeft);
	pushBackVector(m_uvs, topRight);
	pushBackVector(m_uvs, bottomLeft);
	pushBackVector(m_uvs, bottomRight);
}

void CubeMesh::initBuffersAndVAOs()
{
	glGenVertexArrays(NumVAOs, m_VAOs);
	glGenBuffers(NumBuffers, m_buffers);

	initCubeBuffersAndVAOs();
	initCubeConstantBuffersAndVAOs();

	glBindVertexArray(0);
}

void CubeMesh::initCubeBuffersAndVAOs()
{
	constexpr GLsizeiptr offsetVertices = 0;
	const GLsizeiptr sizeOfVertices = static_cast<long>(sizeof(GLfloat) * m_vertices.size());
	const GLsizeiptr sizeOfNormals = static_cast<long>(sizeof(GLfloat) * m_normals.size());
	const GLsizeiptr sizeOfTangents = static_cast<long>(sizeof(GLfloat) * m_tangents.size());
	const GLsizeiptr sizeOfUVs = static_cast<long>(sizeof(GLfloat) * m_uvs.size());
	const GLsizeiptr sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());
	const GLsizeiptr dataSize = sizeOfVertices + sizeOfUVs + sizeOfNormals + sizeOfTangents;

	glBindVertexArray(m_VAOs[VAO_Cube]);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Cube]);
	glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, offsetVertices, sizeOfVertices, m_vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices, sizeOfUVs, m_uvs.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfUVs, sizeOfNormals, m_normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfUVs + sizeOfNormals, sizeOfTangents, m_tangents.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_Cube]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndices, m_indices.data(), GL_STATIC_DRAW);
}

void CubeMesh::initCubeConstantBuffersAndVAOs()
{
	const GLsizeiptr sizeOfVertices = static_cast<long>(sizeof(GLfloat) * m_vertices.size());
	const GLsizeiptr sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());

	glBindVertexArray(m_VAOs[VAO_CubeConstant]);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_CubeConstant]);
	glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_CubeConstant]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndices, m_indices.data(), GL_STATIC_DRAW);
}

