/**
 * @file SceneObject.cpp
 *
 * @brief An object in the scene that has a physical presence as well as a parent and children.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "SceneObject.h"

#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

unsigned int SceneObject::NEXT_ID = 1;

SceneObject::SceneObject()
: m_id(NEXT_ID), m_selected(false), m_name("Object " + std::to_string(m_id-7)), m_transform(*this), m_transform_global(*this), m_transform_global_previous(*this)
{
	if (m_id == 1)
	{
		m_name = std::string("SceneRoot");
		m_canBePicked = false;
		SceneObject::m_root = this;
	}
	
	++NEXT_ID;
}

SceneObject::SceneObject(SceneObject& parent)
	: SceneObject()
{
	m_parent = &parent;
	m_parent->addChild(*this);
}

void SceneObject::render(const Camera& camera, const glm::mat4& previousModelMatrix, const bool worldDirty)
{
	glm::mat4 localModelMatrix;

	//If local dirty, global has been updated. If global dirty, local has been updated. In both cases, the children's global has to be updated as well.
	bool parentDirty = isLocalDirty() || isGlobalDirty() || worldDirty;

	if (isLocalDirty())
	{
		computeLocalTransform();
	}
	
	m_transform.computeModelMatrix(localModelMatrix);
	
	m_modelMatrix = previousModelMatrix * localModelMatrix;

	if (isGlobalDirty() || worldDirty)
	{
		computeGlobalTransform();
	}

	for (const auto child : m_children)
	{
		child->render(camera, m_modelMatrix, parentDirty);
	}

	renderImplementation(camera, m_modelMatrix);
}

void SceneObject::renderId(const Camera& camera, const glm::mat4& previousModelMatrix, bool bypassCanBePicked)
{
	glm::mat4 localModelMatrix;
	
	m_transform.computeModelMatrix(localModelMatrix);

	m_modelMatrix = previousModelMatrix * localModelMatrix;


	for (const auto child : m_children)
	{
		child->renderId(camera, m_modelMatrix);
	}

	if (m_canBePicked || bypassCanBePicked)
	{
		renderIdImplementation(camera, m_modelMatrix);
	}
}

void SceneObject::animate(const float deltaTime)
{
	for (const auto child : m_children)
	{
		child->animate(deltaTime);
	}

	if (m_isAnimatingRotation)
	{
		animateRotation(deltaTime);
	}
}

void SceneObject::stopAnimation()
{
	m_isAnimatingRotation = false;
}

void SceneObject::addRotationAnimation(const glm::vec3& anglesOfRotation, float secondsOfDuration, const glm::vec3& scaleChange)
{
	addRotationAnimation(anglesOfRotation, secondsOfDuration, []() { std::cout << "Animation finished" << std::endl; }, scaleChange);
}

void SceneObject::addRotationAnimation(const glm::vec3& anglesOfRotation, float secondsOfDuration, std::function<void()> rotationFinishedCallback, const glm::vec3& scaleChange)
{
	if (m_isAnimatingRotation)
		return;

	m_isAnimatingRotation = true;
	m_rotationAnimationDuration = secondsOfDuration;
	m_rotationAnimationTimer = 0.0f;

	m_rotationAnimationStart = m_transform.rotation();
	m_rotationAnimationEnd = m_transform.rotation() + anglesOfRotation;

	m_rotationAnimationScaleStart = m_transform.scale();
	m_rotationAnimationScaleEnd = m_rotationAnimationScaleStart + scaleChange;

	m_rotationFinishedCallback = rotationFinishedCallback;

	dirtyGlobal();
}

SceneObject* SceneObject::findChildWithId(unsigned int idToSearch)
{
	if (m_id == idToSearch)
		return this;

	// ReSharper disable once CppLocalVariableMayBeConst
	for (auto child : m_children)
	{
		const auto childWithCorrectId = child->findChildWithId(idToSearch);

		if (childWithCorrectId != nullptr)
			return childWithCorrectId;
	}

	return nullptr;
}

bool SceneObject::isAChild(SceneObject& potentialChild)
{
	for (auto child : m_children)
	{
		if (child == &potentialChild)
		{
			return true;
		}
	}
	return false;
}

void SceneObject::removeParent()
{
	m_parent->removeChild(*this);
	m_parent = nullptr;
}

bool SceneObject::reParent(SceneObject& newParent)
{
	
	if (m_parent)
	{
		if (m_parent == &newParent)
		{
			return false;
		}
		
		if (isAChild(newParent))
		{
			newParent.reParent(*m_parent);
		}
		transformConversion(&newParent);
		m_parent->removeChild(*this);
	}
	else
	{
		if (isAChild(newParent))//Should never happen, only root shouldn't have parents and root shouldn't be reParentable.
		{
			return false;
		}
	}
	
	newParent.addChild(*this);
	m_parent = &newParent;
	
	dirtyGlobal();

	return true;
}

void SceneObject::unselectAllChildren()
{
	unselect();

	for (const auto child : m_children)
	{
		child->unselectAllChildren();
	}
}

void SceneObject::addChild(SceneObject& child)
{
	m_children.push_back(&child);
}

void SceneObject::removeChild(SceneObject& child)
{
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [&child](SceneObject* object) { return object == &child; }));
}

void SceneObject::animateRotation(const float deltaTime)
{
	m_rotationAnimationTimer += deltaTime;
	auto progress = m_rotationAnimationTimer / m_rotationAnimationDuration;

	const auto animationFinished = progress >= 1.0f;
	if (animationFinished)
	{
		m_isAnimatingRotation = false;
		progress = 1.0f;
	}

	const auto sinProgress = sinf(progress * glm::pi<float>() * 0.5f);
	transform().rotation() = (1.0f - sinProgress) * m_rotationAnimationStart + sinProgress * m_rotationAnimationEnd;
	transform().scale() = (1.0f - sinProgress) * m_rotationAnimationScaleStart + sinProgress * m_rotationAnimationScaleEnd;

	dirtyGlobal();

	if (animationFinished)
	{
		m_rotationFinishedCallback();
	}
}


void SceneObject::computeLocalTransform()
{

	if (!m_parent || m_parent == m_root)
	{
		m_transform = m_transform_global;
	}
	else if(m_transform_global != m_transform_global_previous)
	{
		m_transform = m_transform + (m_transform_global- m_transform_global_previous);
		m_transform_global_previous = m_transform_global;
	}
	
	m_dirty_local = false;
}

void SceneObject::computeGlobalTransform()
{
	//fetch parent
	if (!m_parent || m_parent == m_root)
	{
		m_transform_global = m_transform;
	}
	else
	{
		Transform newGlobalTransform(*this);
		newGlobalTransform = newGlobalTransform + m_transform;
		auto addTransformFromHierachy = [&](SceneObject* parent, auto&& addTransformFromHierachy)
		{
			if (parent == nullptr || &parent == &m_root)
			{
				return;
			}
			addTransformFromHierachy(parent->m_parent, addTransformFromHierachy);
			newGlobalTransform = newGlobalTransform + parent->transform();
		};
		addTransformFromHierachy(m_parent, addTransformFromHierachy);
		m_transform_global = newGlobalTransform;
	}
	
	m_transform_global_previous = m_transform_global;
	m_dirty_global = false;
}

void SceneObject::transformConversion(SceneObject* newReference)

{
	m_transform = m_transform_global - newReference->transformGlobal();

	
	//Could try to make it return to its original spot like in Blender or just stay there like in Unreal and Unity

}