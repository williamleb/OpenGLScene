/**
 * @file Camera.cpp
 *
 * @brief Camera that looks at the world from a position and an orientation.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

 // ---------------------------------------------------
 /* Code based on Learn openGL camera class
  *
  * https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/1.getting_started/7.4.camera_class/camera_class.cpp
  * Copyright (C) Joey de Vries -
  * licensed under the terms of the CC BY-NC 4.0 license as published by Creative Commons
  *
  * https://creativecommons.org/licenses/by-nc/4.0/legalcode
  *
  */

#include "Camera.h"

Camera::Camera(int width, int height,
	const glm::vec3& position,
	const glm::vec3& at) :
	m_direction(at-position),
	m_image_ratio(float(width) / height)
{
	m_name = "Camera ";
	m_canBePicked = false;
	m_transform.translation() = position;
	m_direction = glm::normalize(at - position);
	computeAngles();
	updateProjectionMatrix();
}

void Camera::keybordEvents(GLFWwindow* w, const float delta_time)
{
	// amount of moving the camera
	float delta = 12.f * delta_time;
	// compute the perpendicular vector to view direction and up
	const glm::vec3 right = glm::cross(m_direction, m_up);

	// Keyboard displacements
	bool update_position = false;
	if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_transform.translation() += delta * m_direction;
		update_position = true;
	}
	if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_transform.translation() -= delta * m_direction;
		update_position = true;
	}
	if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_transform.translation() -= delta * right;
		update_position = true;
	}
	if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_transform.translation() += delta * right;
		update_position = true;
	}


	if (update_position)
	{
		dirtyGlobal();
		updateProjectionMatrix();
	}
}

void Camera::mouseEvents(const glm::vec2& mousePos, bool clicked)
{
	// Compute offset displacement of mouse position
	glm::vec2 offset = mousePos - m_last_mouse_pos;
	m_last_mouse_pos = mousePos;

	// Apply this offset only if we already capture 
	// on click event (from the previous frame)
	// and we continue to press the mouse button
	if (clicked && m_mouse_was_clicked)
	{
		offset *= 0.2;
		yaw += offset.x;
		pitch -= offset.y;

		// Prevent that camera direction is align with the camera up
		// Note that a camera with quaternion will solve this issue
		pitch = std::max(std::min(pitch, 89.0f), -89.0f);

		m_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		m_direction.y = sin(glm::radians(pitch));
		m_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		m_direction = glm::normalize(m_direction); // Unnecessary, just in case... 

		computeTransformRotation();
		updateProjectionMatrix();
	}
	m_mouse_was_clicked = clicked;
}

void Camera::viewportEvents(int width, int height)
{
	// Update the matrix
	m_image_ratio = float(width) / height;
	updateProjectionMatrix();
}

void Camera::computeTransformRotation()
{
	m_transform.rotation().y = -yaw;
	m_transform.rotation().z = fmod(pitch, 360.f);
	dirtyGlobal();
}

void Camera::computeAngles()
{
	// Horizontal direction
	glm::vec3 h_dir = glm::vec3(m_direction.x, 0.0, -m_direction.z);
	h_dir = glm::normalize(h_dir);
	// Compute yaw
	yaw = glm::degrees(asin(std::abs(h_dir.z)));
	if (h_dir.z >= 0.0)
	{
		if (h_dir.x >= 0.0)
		{
			yaw = 360 - yaw;
		}
		else
		{
			yaw = 180 + yaw;
		}
	}
	else
	{
		if (h_dir.x >= 0.0)
		{
			// Nothing
		}
		else
		{
			yaw = 180 - yaw;
		}
	}
	// Compute pitch
	pitch = glm::degrees(asin(m_direction.y));
}

void Camera::updateProjectionMatrix()
{
	m_proj_matrix = glm::perspective(m_fov, m_image_ratio, zNear(), zFar());
}

void Camera::showEntireScene()
{
	float yview = m_scene_radius / sin(m_fov / 2);
	float xview = m_scene_radius / (atan(tan(m_fov / 2.0) * m_image_ratio));
	float distance = std::max(xview, yview);

	m_transform.translation() = rootTranslation() - distance * m_direction;
	updateProjectionMatrix();
}
