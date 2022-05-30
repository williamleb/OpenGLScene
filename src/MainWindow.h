/**
 * @file MainWindow.h
 *
 * @brief Represents the window application.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "Camera.h"
#include "SceneObject.h"
#include "OBJLoader.h"

class Mesh;
class Material;
class ConstantMaterial;
class SkyboxMaterial;
class TextureMaterial;
class MeshRenderer;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

constexpr int NUMBER_OF_OBJECT_TEXTURES = 7;
inline const char* OBJECT_TEXTURE_NAME[NUMBER_OF_OBJECT_TEXTURES] =
{
	"Grass",
	"Dry Ground",
	"Granite Floor",
	"Limestone Wall",
	"Pierre Boucharde",
	"Wood Floor",
	"No Material"
};
const std::string OBJECT_TEXTURE_PATH[NUMBER_OF_OBJECT_TEXTURES] =
{
	"grass2.jpg",
	"dry_ground.jpg",
	"granite_floor.jpg",
	"limestone_wall.jpg",
	"pierre_bouchardee.jpg",
	"wood_floor.jpg",
	"no_material.png"
};
const std::string OBJECT_TEXTURE_NORMALS_PATH[NUMBER_OF_OBJECT_TEXTURES] =
{
	"grass_normals.jpg",
	"dry_ground_normals.jpg",
	"granite_floor_normals.jpg",
	"limestone_wall_normals.jpg",
	"pierre_bouchardee_normals.jpg",
	"wood_floor_normals.jpg",
	"no_material_normals.png"
};

struct DirectionalLight
{
private:
	float m_intensity = 2.0f;
	float m_radius = 5.0f;
	float m_verticalAngle = 0.8f;
	float m_horizontalAngle = 0.0f;
	float m_biasValue = 0.0005;

	glm::mat4 m_viewProjMatrix;

	float m_near = 0.1f;
	float m_far = 10.0f;
	float m_left = -20.0f;
	float m_right = 20.0f;
	float m_bottom = -20.0f;
	float m_top = 20.0f;

	const int SHADOW_SIZE_X = 2048;
	const int SHADOW_SIZE_Y = 2048;

public:
	float intensity() const { return m_intensity; }
	float radius() const { return m_radius; }
	float verticalAngle() const { return m_verticalAngle; }
	float horizontalAngle() const { return m_horizontalAngle; }
	float biasValue() const { return m_biasValue; }

	float& intensity() { return m_intensity; }
	float& radius() { return m_radius; }
	float& verticalAngle() { return m_verticalAngle; }
	float& horizontalAngle() { return m_horizontalAngle; }
	float& biasValue() { return m_biasValue; }

	float near() const { return m_near; }
	float far() const { return m_far; }
	float left() const { return m_left; }
	float right() const { return m_right; }
	float bottom() const { return m_bottom; }
	float top() const { return m_top; }

	float& near() { return m_near; }
	float& far() { return m_far; }
	float& left() { return m_left; }
	float& right() { return m_right; }
	float& bottom() { return m_bottom; }
	float& top() { return m_top; }

	glm::vec3 position() const
	{
		return glm::vec3(
			m_radius * sinf(m_verticalAngle) * cosf(m_horizontalAngle),
			m_radius * cosf(m_verticalAngle),
			m_radius * sinf(m_verticalAngle) * sinf(m_horizontalAngle)
		);
	}
	glm::vec3 direction() const { return -position(); }
	glm::mat4 viewProjMatrix()
	{
		const glm::mat4 lightOrthoMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);
		constexpr glm::vec3 at(0.0f, 0.0f, 0.0f); // Center of the scene
		constexpr glm::vec3 up(0.0f, 1.0f, 0.0f);
		const glm::mat4 lightViewMatrix = glm::lookAt(position(), at, up);
		return lightOrthoMatrix * lightViewMatrix;
	}

	int shadowSizeX() const { return SHADOW_SIZE_X; };
	int shadowSizeY() const { return SHADOW_SIZE_Y; };
};

class MainWindow
{
public:
	MainWindow();
	~MainWindow();

	int initialisation();
	int renderLoop();

	void frameBufferSizeCallback(int width, int height);
	void mouseButtonCallback(int button, int action, int mods);
	void cursorPositionCallback(double xPos, double yPos);

private:
	void initializeCallback();
	int initializeGL();
	void initializeSceneGraph();
	void initializeSelectionPreviewObject();
	bool loadObjectTextures();
	bool loadTexture(const std::string& path, unsigned int& textureID, GLint uvMode, GLint minMode, GLint magMode);
	bool loadScrewdriver();

    void renderScene();
	void renderSkybox();
	void animate(float deltaTime);
	void renderImGui();

	void updateLightParameters(float deltaTime);
	void updateHoveringFace();

	SceneObject* pickedObjectAt(int x, int y);
	void performSelection(int x, int y);
	void performAddCube();
	void clearColorForPicking();
	unsigned int getPickedIdAtPosition(int x, int y);

	void animateTool();

	MeshRenderer* createNewMeshRenderer(std::shared_ptr<const Mesh> mesh = nullptr, std::shared_ptr<const Material> material = nullptr);
	MeshRenderer* createNewMeshRenderer(SceneObject& parent, std::shared_ptr<const Mesh> mesh = nullptr, std::shared_ptr<const Material> material = nullptr);

private:
	unsigned int m_windowWidth = 1200;
	unsigned int m_windowHeight = 800;

	GLFWwindow* m_window = nullptr;

	
	bool m_imGuiActive = false;

	const float m_animationDuration = 1.0f;

	SceneObject m_root ;
	SceneObject m_environmentSceneObject;
	Camera m_camera;
	SceneObject m_screwDriverSceneObject;
	SceneObject* m_selectedObject = nullptr;
	SceneObject* m_selectionPreviewObject;

	int m_currentObjectTextureIndex = 0;

	OBJLoader::Loader m_screwdriverLoader;

    std::shared_ptr<Mesh> m_cubeMesh;
	std::vector<std::shared_ptr<Mesh>> m_screwdriverMeshes;
    std::shared_ptr<SkyboxMaterial> m_skyDomeMaterial;
    std::shared_ptr<ConstantMaterial> m_constantMaterial;
	std::shared_ptr<TextureMaterial> m_textureMaterial;

	std::vector<const SceneObject*> m_heapSceneObjects;

	bool m_isHoveringFace = false;
	SceneObject* m_hoveringObject = nullptr;
	glm::vec3 m_faceHoveringCenter = glm::vec3(0.0f);
	glm::vec3 m_faceHoveringNormal = glm::vec3(0.0f);

	unsigned int m_objectTextureIDs[NUMBER_OF_OBJECT_TEXTURES];
	unsigned int m_objectNormalsTextureIDs[NUMBER_OF_OBJECT_TEXTURES];

	// Light
	glm::vec3 m_pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_specular = 0.5f;
	float m_pointLightIntensity = 2.0f;

	DirectionalLight m_directionalLight;

	bool m_lightAnimateVertical = false;
	bool m_lightAnimateHorizontal = false;
};
