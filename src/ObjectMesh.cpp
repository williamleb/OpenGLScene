/**
 * @file ObjectMesh.cpp
 *
 * @brief Mesh that takes the shape of a mesh described in an object.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "ObjectMesh.h"

#include <glm/vec3.hpp>

#include "Material.h"
#include "OBJLoader.h"

void ObjectMesh::init(const OBJLoader::Mesh& objectMesh)
{
	initVertices(objectMesh);
	initNormals(objectMesh);
	initIndices(objectMesh);
	initUVs(objectMesh);
	initBuffersAndVAOs();
}

void ObjectMesh::init()
{
	assert(("You should call init(const OBJLoader::Mesh&) instead",false));
}

void ObjectMesh::initAttributes(const std::shared_ptr<const Material>& material) const
{
	glBindVertexArray(m_VAOs[VAO_Object]);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Object]);

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

void ObjectMesh::initConstantAttributes(const std::shared_ptr<const Material>& constantMaterial) const
{
	glBindVertexArray(m_VAOs[VAO_ObjectConstant]);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_ObjectConstant]);

	const int positionLocation = constantMaterial->positionAttribLocation();
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(positionLocation);
}

void ObjectMesh::faceAt(const glm::vec3& position, glm::vec3& outCenter, glm::vec3& outNormal) const
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

void ObjectMesh::bindAndDraw() const
{
	const GLsizei sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());
	glBindVertexArray(m_VAOs[VAO_Object]);
	glDrawElements(GL_TRIANGLES, sizeOfIndices, GL_UNSIGNED_INT, nullptr);
}

void ObjectMesh::bindAndDrawConstant() const
{
	const GLsizei sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());
	glBindVertexArray(m_VAOs[VAO_ObjectConstant]);
	glDrawElements(GL_TRIANGLES, sizeOfIndices, GL_UNSIGNED_INT, nullptr);
}

void ObjectMesh::initVertices(const OBJLoader::Mesh& objectMesh)
{
	for (auto objectVertex : objectMesh.vertices)
	{
		for (auto objectPosition : objectVertex.position)
		{
			m_vertices.push_back(objectPosition);
		}
	}
}

void ObjectMesh::initNormals(const OBJLoader::Mesh& objectMesh)
{
	for (auto objectVertex : objectMesh.vertices)
	{
		for (auto objectNormal : objectVertex.normal)
		{
			m_normals.push_back(objectNormal);
		}
	}
}

void ObjectMesh::initTangents(const OBJLoader::Mesh& objectMesh)
{
	for (auto objectVertex : objectMesh.vertices)
	{
		for (auto objectNormal : objectVertex.normal)
		{
			m_tangents.push_back(0.0f);
		}
	}
}

void ObjectMesh::initIndices(const OBJLoader::Mesh& objectMesh)
{
	for (auto i = 0; i < static_cast<int>(objectMesh.vertices.size()); ++i)
	{
		m_indices.push_back(i);
	}
}

void ObjectMesh::initUVs(const OBJLoader::Mesh& objectMesh)
{
	for (auto objectVertex : objectMesh.vertices)
	{
		for (auto objectUV : objectVertex.uv)
		{
			m_uvs.push_back(objectUV);
		}
	}
}

void ObjectMesh::initBuffersAndVAOs()
{
	glGenVertexArrays(NumVAOs, m_VAOs);
	glGenBuffers(NumBuffers, m_buffers);

	initCubeBuffersAndVAOs();
	initCubeConstantBuffersAndVAOs();

	glBindVertexArray(0);
}

void ObjectMesh::initCubeBuffersAndVAOs()
{
	constexpr GLsizeiptr offsetVertices = 0;
	const GLsizeiptr sizeOfVertices = static_cast<long>(sizeof(GLfloat) * m_vertices.size());
	const GLsizeiptr sizeOfNormals = static_cast<long>(sizeof(GLfloat) * m_normals.size());
	const GLsizeiptr sizeOfTangents = static_cast<long>(sizeof(GLfloat) * m_tangents.size());
	const GLsizeiptr sizeOfUVs = static_cast<long>(sizeof(GLfloat) * m_uvs.size());
	const GLsizeiptr sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());
	const GLsizeiptr dataSize = sizeOfVertices + sizeOfUVs + sizeOfNormals + sizeOfTangents;

	glBindVertexArray(m_VAOs[VAO_Object]);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Object]);
	glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, offsetVertices, sizeOfVertices, m_vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices, sizeOfUVs, m_uvs.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfUVs, sizeOfNormals, m_normals.data());
	glBufferSubData(GL_ARRAY_BUFFER, sizeOfVertices + sizeOfUVs + sizeOfNormals, sizeOfTangents, m_tangents.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_Object]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndices, m_indices.data(), GL_STATIC_DRAW);
}

void ObjectMesh::initCubeConstantBuffersAndVAOs()
{
	const GLsizeiptr sizeOfVertices = static_cast<long>(sizeof(GLfloat) * m_vertices.size());
	const GLsizeiptr sizeOfIndices = static_cast<long>(sizeof(GLfloat) * m_indices.size());

	glBindVertexArray(m_VAOs[VAO_ObjectConstant]);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_ObjectConstant]);
	glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_ObjectConstant]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndices, m_indices.data(), GL_STATIC_DRAW);
}

