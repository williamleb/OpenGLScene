#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

/**
 * @file ShaderProgram.h
 *
 * @brief Translation, rotation and scale of an object in the world.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <glm/glm.hpp>

class SceneObject;

class Transform
{
public:
	Transform(SceneObject& sceneObject, bool dirty = false);
	Transform() = delete;

	inline SceneObject& sceneObject() const { return m_sceneObject; }

	inline glm::vec3& translation() { return m_translation; }
	inline glm::vec3& rotation() { return m_rotation; }
	inline glm::vec3& scale() { return m_scale; }

	inline const glm::vec3& translation() const { return m_translation; }
	inline const glm::vec3& rotation() const { return m_rotation; }
	inline const glm::vec3& scale() const { return m_scale; }

	inline void translation(const glm::vec3& newTranslation) { m_translation = newTranslation; }
	inline void rotation(const glm::vec3& newRotation) { m_rotation = newRotation; }
	inline void scale(const glm::vec3& newScale) { m_scale = newScale; }

	Transform& operator=(const Transform& b);
	Transform operator+(const Transform& b);
	Transform operator-(const Transform& b);
	bool operator==(const Transform& b);
	bool operator!=(const Transform& b);
	

	void computeModelMatrix(glm::mat4& outModelMatrix);

private:
	glm::vec3 m_translation = glm::vec3(0.0f);
	glm::vec3 m_rotation = glm::vec3(0.0f);
	glm::vec3 m_scale = glm::vec3(0.0f);

	SceneObject& m_sceneObject;
};

#endif
