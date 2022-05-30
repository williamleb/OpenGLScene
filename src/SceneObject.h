#pragma once
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

/**
 * @file SceneObject.h
 *
 * @brief An object in the scene that has a physical presence as well as a parent and children.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <vector>
#include <glm/gtc/quaternion.hpp>

#include <functional>
#include <string>
#include "Transform.h"

class Camera;

class SceneObject
{
public:

	SceneObject();
	SceneObject(SceneObject& parent);
	virtual ~SceneObject() = default;

	void render(const Camera& camera, const glm::mat4& previousModelMatrix = glm::mat4(1.0f), const bool worldDirty = false);
	void renderId(const Camera& camera, const glm::mat4& previousModelMatrix = glm::mat4(1.0f), bool bypassCanBePicked = false);
	void animate(const float deltaTime);

	void stopAnimation();
	void addRotationAnimation(const glm::vec3& anglesOfRotation, float secondsOfDuration, const glm::vec3& scaleChange = glm::vec3(0.0f));
	void addRotationAnimation(const glm::vec3& anglesOfRotation, float secondsOfDuration, std::function<void()> rotationFinishedCallback, const glm::vec3& scaleChange = glm::vec3(0.0f));

	SceneObject* findChildWithId(unsigned int idToSearch);
	bool isAChild(SceneObject& potentialChild);

	void removeParent();
	bool reParent(SceneObject& newParent);


	//Local Transform
	inline Transform& transform() { return m_transform; }
	inline const Transform& transform() const { return m_transform; }
	inline glm::vec3& translation() {  return m_transform.translation(); }
	inline const glm::vec3& translation() const { return m_transform.translation(); }
	inline glm::vec3& rotation() {  return m_transform.rotation(); }
	inline const glm::vec3& rotation() const { return m_transform.rotation(); }
	inline glm::vec3& scale() { return m_transform.scale(); }
	inline const glm::vec3& scale() const { return m_transform.scale(); }

	inline const glm::vec3 rootTranslation() const { return m_root->m_transform.translation(); }


	//Global Transform
	inline Transform& transformGlobal() { return m_transform_global; }
	inline const Transform& transformGlobal() const { return m_transform_global; }
	inline glm::vec3& translationGlobal() { return m_transform_global.translation(); }
	inline const glm::vec3& translationGlobal() const { return m_transform_global.translation(); }
	inline glm::vec3& rotationGlobal() { return m_transform_global.rotation(); }
	inline const glm::vec3& rotationGlobal() const { return m_transform_global.rotation(); }
	inline glm::vec3& scaleGlobal() { return m_transform_global.scale(); }
	inline const glm::vec3& scaleGlobal() const { return m_transform_global.scale(); }
	

	inline const glm::mat4& modelMatrix() const { return m_modelMatrix; }

	inline std::vector<SceneObject*>& children() { return m_children; }
	inline const std::vector<SceneObject*>& children() const { return m_children; }

	inline const std::string& getName() const { return m_name; }
	inline void setName(const std::string& newName) { m_name = newName; }

	inline unsigned int id() const { return m_id; }


	inline bool selected() const { return m_selected; }
	inline void select() { m_selected = true; }
	inline void unselect() { m_selected = false; }
	void unselectAllChildren();

	inline void canBePicked(const bool value) { m_canBePicked = value; }
	inline const bool canBePicked() {return m_canBePicked;}

	/**
	 * To be called whenever there's an outside change to the global transform to update the local transform.
	 */
	inline void dirtyLocal() { m_dirty_local = true; }

	/**
	 * To be called whenever there's an outside change to the local transform to update the global transform.
	 */
	inline void dirtyGlobal() { m_dirty_global = true; }

protected:


	virtual void renderImplementation(const Camera& camera, const glm::mat4& modelMatrix) {}
	virtual void renderIdImplementation(const Camera& camera, const glm::mat4& modelMatrix) {}

private:
	void addChild(SceneObject& child);
	void removeChild(SceneObject& child);

	void animateRotation(const float deltaTime);

	/**
	 * Recompute the local transform to accommodate the global transform's changes.
	 */
	void computeLocalTransform();

	/**
	 * Recompute the global transform to accommodate the local transform's changes.
	 */
	void computeGlobalTransform();
	
	/**
	 * Local is dirty from the moment when the global transform is modified up
	 * to the local transform is recomputed.
	 */
	bool isLocalDirty() { return m_dirty_local; }

	/**
	 * Global is dirty from the moment when the local transform is modified up
	 * to the global transform is recomputed.
	 */
	bool isGlobalDirty() { return m_dirty_global; }
	
	/**
	 * Generates a new transform for the calling object in the passed-in object's local space.
	 */
	void transformConversion(SceneObject* newReference);

protected:
	unsigned int m_id;
	bool m_selected;
	bool m_canBePicked = true;
	std::string m_name;

	SceneObject* m_parent = nullptr;
	std::vector<SceneObject*> m_children;
	
	//every object has access to root once it is created. (Requires C++ 17)
	inline static SceneObject* m_root = nullptr;

	Transform m_transform;
	Transform m_transform_global;
	Transform m_transform_global_previous;

	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	bool m_dirty_local = false;
	bool m_dirty_global = true;

	glm::vec3 m_rotationAnimationStart = glm::vec3(0.0f);
	glm::vec3 m_rotationAnimationEnd = glm::vec3(0.0f);
	glm::vec3 m_rotationAnimationScaleStart = glm::vec3(1.0f);
	glm::vec3 m_rotationAnimationScaleEnd = glm::vec3(1.0f);
	bool m_isAnimatingRotation = false;
	float m_rotationAnimationDuration = 0.0f;
	float m_rotationAnimationTimer = 0.0f;
	std::function<void()> m_rotationFinishedCallback;

	unsigned static int NEXT_ID;
};

#endif
