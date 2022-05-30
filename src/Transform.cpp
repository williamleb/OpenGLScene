/**
 * @file ShaderProgram.cpp
 *
 * @brief Translation, rotation and scale of an object in the world.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "SceneObject.h"
#include "ExtraOperators.h"

Transform::Transform(SceneObject& sceneObject, bool dirty)
	: m_translation(0.0f), m_rotation(0.0f), m_scale(1.0f), m_sceneObject(sceneObject)
{ }

Transform& Transform::operator=(const Transform& b)
{
	m_translation = b.translation();
	m_rotation = b.rotation();
	m_scale = b.scale();

	return *this;
}

Transform Transform::operator+(const Transform& b)
{
	Transform sum(m_sceneObject); //TODO FIX HACK/ TERRIBLE DESIGN SORRY

	sum.translation() = m_translation + b.translation();
	sum.rotation() = m_rotation + b.rotation();
	sum.scale() = m_scale;

	return sum;
}

Transform Transform::operator-(const Transform& b)
{
	Transform substraction(m_sceneObject); //TODO FIX HACK / TERRIBLE DESIGN SORRY

	substraction.translation() = m_translation - b.translation();
	substraction.rotation() = m_rotation - b.rotation();
	substraction.scale() = m_scale;

	return substraction;
}

bool Transform::operator==(const Transform& b)
{
	return ((this->translation() == b.translation())
		&& (this->rotation() == b.rotation())
		&& (this->scale() == b.scale()));
}

bool Transform::operator!=(const Transform& b)
{
	return ((this->translation() != b.translation())
		|| (this->rotation() != b.rotation())
		|| (this->scale() != b.scale()));
}

void Transform::computeModelMatrix(glm::mat4& outModelMatrix)
{
	outModelMatrix = glm::mat4(1.0f);
	outModelMatrix = glm::translate(outModelMatrix, m_translation);
	outModelMatrix = outModelMatrix * glm::eulerAngleXYZ(glm::radians(m_rotation.x), glm::radians(m_rotation.y), glm::radians(m_rotation.z));
	outModelMatrix = glm::scale(outModelMatrix, m_scale);
}
