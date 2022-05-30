#pragma once 

/**
 * @file Camera.h
 *
 * @brief Camera that looks at the world from a position and an orientation.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>

#include "SceneObject.h"

class Camera : public SceneObject
{
public:
	// Create camera with width and height window
	// at a given position and looking at a point
	Camera(int width, int height,
		const glm::vec3& position,
		const glm::vec3& at);

	// Methods processing GLFW inputs
	void keybordEvents(GLFWwindow* w, const float delta_time);
	void mouseEvents(const glm::vec2& mousePos, bool clicked);
	void viewportEvents(int width, int height);

	// Compute view matrix
	glm::mat4 viewMatrix() const
	{
		return glm::lookAt(m_transform.translation(), m_transform.translation() + m_direction, m_up);
	}
	// Compute (perspective) projection matrix
	glm::mat4 projectionMatrix() const
	{
		return m_proj_matrix;
	}

	// Update scene radius and center
	// These values needs to be updated 
	void setSceneCenter(const glm::vec3& center)
	{
		m_scene_center = center;
		updateProjectionMatrix();
	}
	void setSceneRadius(const float r)
	{
		m_scene_radius = r;
		updateProjectionMatrix();
	}

	void showEntireScene();
	const glm::vec3& position() const { return m_transform.translation(); }
	float fieldOfView() const { return m_fov; }
    
protected:
	void computeTransformRotation();
    
private:
	// Compute yaw and vertical angles for the view direction
	void computeAngles();

	// Methods to compute zNear and zFar
	inline float distanceToSceneCenter() const
	{
		glm::vec3 v = m_scene_center - translation();
		return std::abs(glm::dot(m_direction, v));
	}
	inline float zNear() const
	{
		const float zMin = 0.005;

		float zNearScene = m_scene_radius * std::sqrt(3.0);
		float z = distanceToSceneCenter() - zNearScene;
		return std::max(z, zMin);
	}
	inline float zFar() const
	{
		return distanceToSceneCenter() + m_scene_radius * std::sqrt(3.0);
	}
	void updateProjectionMatrix();

private:
	
	// - camera up vector
	const glm::vec3 m_up = glm::vec3(0, 1, 0);

	glm::vec3 m_direction;
	
	// Projection matrix
	const float m_fov = glm::radians(45.0f);
	glm::vec3 m_scene_center = glm::vec3(0.0);
	float m_scene_radius = 1.0;
	float m_image_ratio;
	glm::mat4 m_proj_matrix;

    // Fix near and far
    bool m_nearFarFixed = false;

	// Orientation in degrees
	float yaw;
	float pitch;

	// Mouse position tracking
	bool m_mouse_was_clicked = false;
	glm::vec2 m_last_mouse_pos;
};