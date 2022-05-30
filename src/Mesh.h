#pragma once
#ifndef MESH_H
#define MESH_H

/**
 * @file MainWindow.h
 *
 * @brief A mesh defined by its vertices, indexes, normals and faces.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <memory>
#include <glad/glad.h>
#include <vector>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class Material;

class Mesh
{
public:
	virtual ~Mesh() = default;

	virtual void init() = 0;
	virtual void initAttributes(const std::shared_ptr<const Material>& material) const = 0;
	virtual void initConstantAttributes(const std::shared_ptr<const Material>& constantMaterial) const = 0;

	virtual void faceAt(const glm::vec3& position, glm::vec3& outCenter, glm::vec3& outNormal) const = 0;

	virtual const std::vector<GLfloat>& vertices() = 0;
	virtual const std::vector<GLfloat>& normals() = 0;
	virtual const std::vector<GLfloat>& tangents() = 0;
	virtual const std::vector<GLfloat>& uvs() = 0;
	virtual const std::vector<GLuint>& indices() = 0;

	virtual void bindAndDraw() const = 0;
	virtual void bindAndDrawConstant() const = 0;
};

#endif
