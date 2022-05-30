/**
 * @file MainWindow.cpp
 *
 * @brief Represents the window application.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "MainWindow.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <thread>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TextureMaterial.h"
#include "SkyboxMaterial.h"
#include "ConstantMaterial.h"
#include "CubeMesh.h"
#include "ObjectMesh.h"
#include "MeshRenderer.h"

inline uint32_t getIntFromRGBA(const glm::uvec4& rgba)
{
	return (rgba.a << 24) + (rgba.r << 16) + (rgba.g << 8) + rgba.b;
}

MainWindow::MainWindow() :
	m_camera(static_cast<int>(m_windowWidth), static_cast<int>(m_windowHeight),
	         glm::vec3(4.0, 2.0, 4.0),
	         glm::vec3(0.0, 0.0, 0.0)),
	m_environmentSceneObject(m_root),
	m_screwDriverSceneObject(m_camera)
{
	m_root.setName("ObjectRoot");
	m_root.canBePicked(false);

	m_environmentSceneObject.setName("Environment");

	m_screwDriverSceneObject.setName("Screwdriver");
	m_screwDriverSceneObject.canBePicked(true); // TRUE ONLY FOR DEV, can change values with ImGui :p
	m_screwDriverSceneObject.transform().scale(glm::vec3(0.05f));
	m_screwDriverSceneObject.transform().translation(glm::vec3(0.40f, -0.1f, 0.15f));
    
	m_camera.reParent(m_root);
}

MainWindow::~MainWindow()
{
	for (const auto* sceneObject : m_heapSceneObjects)
	{
		delete sceneObject;
	}

	m_screwdriverLoader.unload();
}

int MainWindow::initialisation()
{
	const char* glslVersion = "#version 430 core";

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	m_window = glfwCreateWindow(static_cast<int>(m_windowWidth), static_cast<int>(m_windowHeight), "Labo 3", nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(m_window);
	initializeCallback();

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return 2;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	if (!loadObjectTextures())
	{
		return 4;
	}

	const auto glError = initializeGL();
	if (glError != 0)
		return glError;

	if (!loadScrewdriver())
	{
		return 4;
	}

	initializeSelectionPreviewObject();
	initializeSceneGraph();


	return 0;
}

void MainWindow::initializeCallback()
{
	glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			auto* w = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
			w->frameBufferSizeCallback(width, height);
		});
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto* w = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
			w->mouseButtonCallback(button, action, mods);
		});

	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
		{
			auto* w = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
			w->cursorPositionCallback(xpos, ypos);
		});

}

int MainWindow::initializeGL()
{
    m_skyDomeMaterial = std::make_shared<SkyboxMaterial>();
    if (!m_skyDomeMaterial->init()){
        return 3;
    }

	m_textureMaterial = std::make_shared<TextureMaterial>();
	if (!m_textureMaterial->init())
	{
		return 3;
	}

	m_constantMaterial = std::make_shared<ConstantMaterial>();
	if (!m_constantMaterial->init())
	{
		return 3;
	}

	m_cubeMesh = std::make_shared<CubeMesh>();
	m_cubeMesh->init();
	m_cubeMesh->initAttributes(m_textureMaterial);
	m_cubeMesh->initConstantAttributes(m_constantMaterial);

    m_camera.setSceneRadius(3.0);
    m_camera.showEntireScene();

	// Init GL properties
	glPointSize(10.0f);
	glEnable(GL_DEPTH_TEST);

	// Setup projection matrix (a bit hacky)
	frameBufferSizeCallback(static_cast<int>(m_windowWidth), static_cast<int>(m_windowHeight));

	return 0;
}

constexpr int numberOfHorizontalCubes = 5;
constexpr int numberOfVerticalCubes = 5;
void MainWindow::initializeSceneGraph()
{
	m_camera.setSceneCenter(glm::vec3(0));
	m_camera.setSceneRadius(20.0); //TODO ?Fixed? need review.

	auto evenHorizontalAlign = numberOfHorizontalCubes % 2 == 0 ? 0.5f : 0.0f;
	auto evenVerticalAlign = numberOfVerticalCubes % 2 == 0 ? 0.5f : 0.0f;


	m_camera.reParent(m_root);

	for (auto i = 0; i < numberOfHorizontalCubes; ++i)
	{
		for (auto j = 0; j < numberOfVerticalCubes; ++j)
		{
			const auto posX = i - numberOfHorizontalCubes / 2;
			const auto posZ = j - numberOfVerticalCubes / 2;

			auto* newRenderer = createNewMeshRenderer(m_environmentSceneObject);
			newRenderer->transform().translation(glm::vec3(posX + evenHorizontalAlign, 0, posZ + evenVerticalAlign));
		}
	}
}

void MainWindow::initializeSelectionPreviewObject()
{
	const auto selectionObject = createNewMeshRenderer();
	selectionObject->selectedColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	selectionObject->canBePicked(false);
	selectionObject->select();

	m_selectionPreviewObject = selectionObject;
}

bool MainWindow::loadObjectTextures()
{
	const std::string assetsDir = ASSETS_DIR;
	for (auto i = 0; i < NUMBER_OF_OBJECT_TEXTURES; ++i)
	{
		std::string imagePath = assetsDir + OBJECT_TEXTURE_PATH[i];
		if (!loadTexture(imagePath, m_objectTextureIDs[i], GL_CLAMP_TO_BORDER, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR))
		{
			std::cerr << "Unable to load texture: " << imagePath << std::endl;
			return false;
		}
		else
		{
			std::cout << "Load texture " << OBJECT_TEXTURE_NAME[i] << " -- OpenGL ID: " << m_objectTextureIDs[i] << "\n";
		}

		std::string normalsImagePath = assetsDir + OBJECT_TEXTURE_NORMALS_PATH[i];
		if (!loadTexture(normalsImagePath, m_objectNormalsTextureIDs[i], GL_CLAMP_TO_BORDER, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR))
		{
			std::cerr << "Unable to load texture: " << normalsImagePath << std::endl;
			return false;
		}
		else
		{
			std::cout << "Load normals texture " << OBJECT_TEXTURE_NAME[i] << " -- OpenGL ID: " << m_objectNormalsTextureIDs[i] << "\n";
		}
	}

	return true;
}

bool MainWindow::loadTexture(const std::string& path, unsigned int& textureID, GLint uvMode, GLint minMode, GLint magMode)
{
	glGenTextures(1, &textureID);

	// Ask the library to flip the image horizontally
	// This is necessary as TexImage2D assume "The first element corresponds to the lower left corner of the texture image"
	// whereas stb_image load the image such "the first pixel pointed to is top-left-most in the image"
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, STBI_rgb_alpha);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		if (minMode == GL_LINEAR_MIPMAP_LINEAR) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uvMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uvMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);

		stbi_image_free(data);

		std::cout << "Texture loaded at path: " << path << std::endl;
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return false;
	}

	return true;
}

bool MainWindow::loadScrewdriver()
{
	const std::string assetsDir = ASSETS_DIR;
	const std::string modelPath = assetsDir + "tournevis.obj";
	if (!m_screwdriverLoader.loadFile(modelPath))
	{
		std::cout << "Failed to load screwdriver file " << modelPath << std::endl;
		return false;
	}

	const auto currentTextureIndex = m_currentObjectTextureIndex;
	m_currentObjectTextureIndex = NUMBER_OF_OBJECT_TEXTURES - 1; // No material

	for (const auto& mesh : m_screwdriverLoader.getMeshes())
	{
		auto objectMesh = std::make_shared<ObjectMesh>();
		objectMesh->init(mesh);
		objectMesh->initAttributes(m_textureMaterial);
		objectMesh->initConstantAttributes(m_constantMaterial);

		const auto meshRenderer = createNewMeshRenderer(m_screwDriverSceneObject, objectMesh, m_textureMaterial);
		meshRenderer->setName(mesh.name);
		meshRenderer->canBePicked(false);
		meshRenderer->setColorsFromObjectLoader(m_screwdriverLoader, mesh.materialID);
	}

	m_currentObjectTextureIndex = currentTextureIndex;

	return true;
}

void MainWindow::renderImGui()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//imgui 
	{

		//ImGui::ShowDemoWindow();
		ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(20, 100), ImGuiCond_Once);
		ImGui::Begin("Inspector");

		ImGui::Combo("Type of cube", &m_currentObjectTextureIndex, OBJECT_TEXTURE_NAME, IM_ARRAYSIZE(OBJECT_TEXTURE_NAME));

		auto selectedObjectInputs = [&](SceneObject* objectToTransform, const float animDuration)
		{
			if (objectToTransform != nullptr)
			{
				ImGui::Separator();

				ImGui::Text("Selected object: ");

				auto name = objectToTransform->getName();
				char nameBuffer[64];
				auto i = 0;
				for (; i < 64 && i < static_cast<int>(name.size()); ++i)
				{
					nameBuffer[i] = name.at(i);
				}
				for (; i < 64; ++i)
				{
					nameBuffer[i] = ' ';
				}

				ImGui::InputText("Name", nameBuffer, 64);
				objectToTransform->setName(std::string(nameBuffer));

				//Disable non-pickable items modification. Beginning.
				bool couldBePicked = true;
				if (!objectToTransform->canBePicked())
				{
					ImGui::BeginDisabled();
					couldBePicked = false;
				}

				//Signal if there's a change to the local transform update the global transform.
				bool isGlobalDirty = false;

				//Update Local Transform
				ImGui::Text("Local Transform: ");
				isGlobalDirty = ImGui::InputFloat3("Translate object", &objectToTransform->translation()[0], "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				isGlobalDirty = isGlobalDirty || ImGui::InputFloat3("Rotate object", &objectToTransform->rotation()[0], "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				isGlobalDirty = isGlobalDirty || ImGui::InputFloat3("Scale object", &objectToTransform->scale()[0], "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

				if (isGlobalDirty)
				{
					objectToTransform->dirtyGlobal();
				}

				//Signal if there's a change to the global transform update the local transform.
				bool isLocalDirty = false;

				//Update Global Transform.
				ImGui::Text("Global Transform: ");
				isLocalDirty = ImGui::InputFloat3("Translate object ##Global", &objectToTransform->translationGlobal()[0], "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				isLocalDirty = isLocalDirty || ImGui::InputFloat3("Rotate object ##Global", &objectToTransform->rotationGlobal()[0], "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
				isLocalDirty = isLocalDirty || ImGui::InputFloat3("Scale object ##Global", &objectToTransform->scaleGlobal()[0], "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

				if (isLocalDirty)
				{
					objectToTransform->dirtyLocal();
				}


				ImGui::Text("Animate rotations: ");
				// X
				const bool rotationXN = ImGui::Button("X-");
				ImGui::SameLine();
				const bool rotationXP = ImGui::Button("X+");
				// Y
				const bool rotationYN = ImGui::Button("Y-");
				ImGui::SameLine();
				const bool rotationYP = ImGui::Button("Y+");
				// Z
				const bool rotationZN = ImGui::Button("Z-");
				ImGui::SameLine();
				const bool rotationZP = ImGui::Button("Z+");


				if (rotationXN)
				{
					objectToTransform->addRotationAnimation(glm::vec3(-90.0f, 0.0f, 0.0f), animDuration);
				}
				if (rotationYN)
				{
					objectToTransform->addRotationAnimation(glm::vec3(0.0f, -90.0f, 0.0f), animDuration);
				}
				if (rotationZN)
				{
					objectToTransform->addRotationAnimation(glm::vec3(0.0f, 0.0f, -90.0f), animDuration);
				}
				if (rotationXP)
				{
					objectToTransform->addRotationAnimation(glm::vec3(90.0f, 0.0f, 0.0f), animDuration);
				}
				if (rotationYP)
				{
					objectToTransform->addRotationAnimation(glm::vec3(0.0f, 90.0f, 0.0f), animDuration);
				}
				if (rotationZP)
				{
					objectToTransform->addRotationAnimation(glm::vec3(0.0f, 0.0f, 90.0f), animDuration);
				}

				if (objectToTransform != &m_root)
				{
					ImGui::Text("Operations: ");

					if (ImGui::Button("Delete"))
					{
						objectToTransform->addRotationAnimation(glm::vec3(0.0f, 360.0f, 0.0f), 0.5f, [objectToTransform]()
							{
								objectToTransform->removeParent();
							}, -objectToTransform->scale());

						objectToTransform->canBePicked(false);
						m_selectedObject = nullptr;
					}
				}

				//Disable Non-pickable items modification, End.
				if (!couldBePicked)
				{
					ImGui::EndDisabled();
				}


				if (auto* meshRenderer = dynamic_cast<MeshRenderer*>(objectToTransform))
				{
					ImGui::Text("Color:");
					ImGui::ColorEdit3("Ambiant color", &meshRenderer->ambiantColor()[0]);
					ImGui::ColorEdit3("Diffuse color", &meshRenderer->diffuseColor()[0]);
					ImGui::ColorEdit3("Specular color", &meshRenderer->specularColor()[0]);
					ImGui::InputFloat("Specular term", &meshRenderer->specularTerm());
				}

			}

			ImGui::Separator();
			ImGui::Text("Light settings");
			ImGui::ColorEdit3("Point light color", &m_pointLightColor[0]);
			ImGui::InputFloat("Point light Intensity", &m_pointLightIntensity);
			ImGui::SliderFloat("Specular", &m_specular, 0, 1);

			ImGui::InputFloat("Intensity", &m_directionalLight.intensity());

			ImGui::InputFloat("Radius", &m_directionalLight.radius(), 0.1f);
			ImGui::InputFloat("Vertical angle", &m_directionalLight.verticalAngle(), 0.05f);
			ImGui::InputFloat("Horizontal angle", &m_directionalLight.horizontalAngle(), 0.05f);

			ImGui::InputFloat("Bias", &m_directionalLight.biasValue());

			ImGui::InputFloat("Near", &m_directionalLight.near());
			ImGui::InputFloat("Far", &m_directionalLight.far());
			ImGui::InputFloat("Left", &m_directionalLight.left());
			ImGui::InputFloat("Right", &m_directionalLight.right());
			ImGui::InputFloat("Bottom", &m_directionalLight.bottom());
			ImGui::InputFloat("Top", &m_directionalLight.top());

			ImGui::Checkbox("Animate vertical", &m_lightAnimateVertical);
			ImGui::Checkbox("Animate horizontal", &m_lightAnimateHorizontal);

			m_textureMaterial->setLightColor(m_pointLightColor);
			m_textureMaterial->setDirectionalLightOrientation(m_directionalLight.direction());
			m_textureMaterial->setLightIntensity(m_pointLightIntensity);
			m_textureMaterial->setDirectionalLightIntensity(m_directionalLight.intensity());
			m_textureMaterial->setSpecular(m_specular);

			ImGui::Separator();
		};

		selectedObjectInputs(m_selectedObject, m_animationDuration);

		ImGui::End();

		ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(m_windowWidth) - 220, 100), ImGuiCond_Once);
		ImGui::Begin("Hierarchy");

		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("SceneGraph"))
		{

			auto sceneTree = [&](SceneObject* sceneObject, SceneObject*& selectedObject, auto&& sceneTree)
			{
				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);

				ImGuiTreeNodeFlags treenode_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				ImGuiDragDropFlags drag_drop_flags = ImGuiDragDropFlags_SourceAllowNullID;
				bool isSelected = false;


				if (selectedObject == sceneObject)
				{
					isSelected = true;
					selectedObject->select();
					treenode_flags |= ImGuiTreeNodeFlags_Selected;
				}

				if (sceneObject->children().empty())
				{
					treenode_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;


					ImGui::TreeNodeEx(sceneObject, treenode_flags, sceneObject->getName().c_str());
					if (ImGui::IsItemClicked())
					{
						if (selectedObject)
						{
							selectedObject->unselect();
						}
						selectedObject = sceneObject;
					}
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						unsigned int objectID = sceneObject->id();
						ImGui::SetDragDropPayload("NODE_CHILD", &objectID, sizeof(unsigned int));
						ImGui::Text("Drag to a new parent.");
						ImGui::EndDragDropSource();

					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("NODE_CHILD"))
						{

							IM_ASSERT(payload->DataSize == sizeof(unsigned int));
							unsigned int receivedPayload = *(const unsigned int*)payload->Data;

							SceneObject* payloadObject = m_root.findChildWithId(receivedPayload);

							payloadObject->reParent(*sceneObject);

						}
						ImGui::EndDragDropTarget();
					}

					return;
				}
				else
				{
					bool node_open = ImGui::TreeNodeEx(sceneObject, treenode_flags, sceneObject->getName().c_str());

					if (ImGui::IsItemClicked())
					{
						if (selectedObject)
						{
							selectedObject->unselect();
						}
						selectedObject = sceneObject;
					}

					if (selectedObject && selectedObject->canBePicked() && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						unsigned int objectID = sceneObject->id();
						ImGui::SetDragDropPayload("NODE_CHILD", &objectID, sizeof(unsigned int));
						ImGui::Text("Drag to a new parent.");
						ImGui::EndDragDropSource();
					}
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("NODE_CHILD"))
						{

							IM_ASSERT(payload->DataSize == sizeof(unsigned int));
							unsigned int receivedPayload = *(const unsigned int*)payload->Data;

							SceneObject* payloadObject = m_root.findChildWithId(receivedPayload);

							payloadObject->reParent(*sceneObject);

						}
					}

					if (node_open)
					{
						for (auto child : sceneObject->children())
						{
							sceneTree(child, selectedObject, sceneTree);
						}

						ImGui::TreePop();
					}
					return;
				}

			};
			sceneTree(&m_root, m_selectedObject, sceneTree);


		}



		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainWindow::updateLightParameters(float deltaTime)
{
	if (m_lightAnimateVertical)
	{
		m_directionalLight.verticalAngle() += 0.5f * deltaTime;
	}

	if (m_lightAnimateHorizontal)
	{
		m_directionalLight.horizontalAngle() += 0.5f * deltaTime;
	}
}

void MainWindow::updateHoveringFace()
{
	double doubleX, doubleY;
	glfwGetCursorPos(m_window, &doubleX, &doubleY);

	auto x = static_cast<float>(doubleX);
	auto y = static_cast<float>(doubleY);

	auto pickedObject = pickedObjectAt(x, y);
	if (pickedObject == nullptr)
	{
		m_isHoveringFace = false;
		return;
	}

	const auto pickedMeshRenderer = dynamic_cast<MeshRenderer*>(pickedObject);
	if (pickedMeshRenderer == nullptr)
	{
		m_isHoveringFace = false;
		return;
	}

	float depth = 0;
	glReadPixels(static_cast<int>(x), static_cast<int>(m_windowHeight) - 1 - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	if (depth >= 1)
	{
		m_isHoveringFace = false;
		return;
	}

	const glm::vec3 windowPoint(x, static_cast<float>(m_windowHeight) - 1 - y, depth);
	const glm::vec4 viewport(0, 0, m_windowWidth, m_windowHeight);
	const glm::vec3 worldPoint = glm::unProject(windowPoint, m_camera.viewMatrix(), m_camera.projectionMatrix(), viewport);

	glm::vec3 center;
	glm::vec3 normal;
	pickedMeshRenderer->getFace(worldPoint, center, normal);

	m_isHoveringFace = true;
	m_faceHoveringCenter = center;
	m_faceHoveringNormal = normal;
	m_hoveringObject = pickedObject;
}

SceneObject* MainWindow::pickedObjectAt(int x, int y)
{
	clearColorForPicking();
	m_root.renderId(m_camera);
	glFinish();

	const auto pickedObjectId = getPickedIdAtPosition(x, y);

	return m_root.findChildWithId(pickedObjectId);
}

void MainWindow::performSelection(int x, int y)
{
	m_selectedObject = pickedObjectAt(x, y);

	m_root.unselectAllChildren();
	if (m_selectedObject != nullptr)
	{
		m_selectedObject->select();
	}
}

void MainWindow::performAddCube()
{
	if (!m_isHoveringFace)
		return;

	const glm::vec3 newCubeTranslation = m_faceHoveringCenter + 0.5f * m_faceHoveringNormal;

	auto* newCube = createNewMeshRenderer(*m_hoveringObject);
	newCube->transform().translation(newCubeTranslation);

	m_root.unselectAllChildren();
	m_selectedObject = newCube;
	newCube->select();

	animateTool();
}

void MainWindow::animateTool()
{
	m_screwDriverSceneObject.stopAnimation();
	m_screwDriverSceneObject.rotation() = glm::vec3(0.0f);

	m_screwDriverSceneObject.addRotationAnimation(glm::vec3(0.0f, 0.0f, -90.0f), 0.1f, [this]()
		{
			m_screwDriverSceneObject.addRotationAnimation(glm::vec3(0.0f, 0.0f, 90.0f), 0.2f);
		});
}

void MainWindow::clearColorForPicking()
{
	float clearColor[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
}

unsigned MainWindow::getPickedIdAtPosition(int x, int y)
{
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char pixelData[4];
	glReadPixels(x, static_cast<int>(m_windowHeight) - 1 - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	const glm::uvec4 pickedColor(pixelData[0], pixelData[1], pixelData[2], pixelData[3]);
	return getIntFromRGBA(pickedColor);
}

MeshRenderer* MainWindow::createNewMeshRenderer(std::shared_ptr<const Mesh> mesh, std::shared_ptr<const Material> material)
{
	if (mesh == nullptr)
		mesh = m_cubeMesh;

	if (material == nullptr)
		material = m_textureMaterial;

	auto* meshRenderer = new MeshRenderer(mesh, material, m_constantMaterial);
	m_heapSceneObjects.push_back(meshRenderer);
	return meshRenderer;
}

MeshRenderer* MainWindow::createNewMeshRenderer(SceneObject& parent, std::shared_ptr<const Mesh> mesh, std::shared_ptr<const Material> material)
{
	if (mesh == nullptr)
		mesh = m_cubeMesh;

	if (material == nullptr)
		material = m_textureMaterial;

	auto* meshRenderer = new MeshRenderer(parent, mesh, material, m_constantMaterial);

	const auto textureIndex = m_objectTextureIDs[m_currentObjectTextureIndex];
	const auto normalsTextureIndex = m_objectNormalsTextureIDs[m_currentObjectTextureIndex];
	meshRenderer->textureIndex(textureIndex);
	meshRenderer->normalsTextureIndex(normalsTextureIndex);

	m_heapSceneObjects.push_back(meshRenderer);
	return meshRenderer;
}

void MainWindow::renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderSkybox();

	m_root.render(m_camera);

	if (m_isHoveringFace && !glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL))
	{
		const glm::vec3 newCubeTranslation = m_faceHoveringCenter + 0.5f * m_faceHoveringNormal;

		m_selectionPreviewObject->transform().translation() = newCubeTranslation;
		m_selectionPreviewObject->render(m_camera, m_hoveringObject->modelMatrix());
	}
}

void MainWindow::renderSkybox()
{
    glDisable(GL_DEPTH_TEST);
    m_skyDomeMaterial->bind();
    m_skyDomeMaterial->setProjMatrix(m_camera.projectionMatrix());
    m_skyDomeMaterial->setViewRotMatrix(glm::mat3(m_camera.viewMatrix()));
    m_skyDomeMaterial->setSunPos(m_directionalLight.position());
    m_skyDomeMaterial->setSunRot(glm::vec2(m_directionalLight.horizontalAngle(),m_directionalLight.verticalAngle()));
    m_skyDomeMaterial->setViewDir(m_camera.rotation());
    m_skyDomeMaterial->bindTextureAndDraw();
    glEnable(GL_DEPTH_TEST);
}

void MainWindow::animate(float deltaTime)
{
	m_root.animate(deltaTime);
}

int MainWindow::renderLoop()
{
	auto time = static_cast<float>(glfwGetTime());
	while (!glfwWindowShouldClose(m_window))
	{
		// Compute delta time between two frames
		const auto newTime = static_cast<float>(glfwGetTime());
		const float deltaTime = newTime - time;
		time = newTime;

		// Check inputs: Does ESC was pressed?
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_window, true);
		if (!m_imGuiActive)
		{
			m_camera.keybordEvents(m_window, deltaTime);
		}

		updateLightParameters(deltaTime);
		updateHoveringFace();
		animate(deltaTime);
		renderScene();
		renderImGui();

		// Show rendering and get events
		glfwSwapBuffers(m_window);
		m_imGuiActive = ImGui::IsAnyItemActive();
		glfwPollEvents();
	}

	// Cleanup
	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}

void MainWindow::frameBufferSizeCallback(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;
	glViewport(0, 0, width, height);
	m_camera.viewportEvents(width, height);
}

void MainWindow::cursorPositionCallback(double xPos, double yPos)
{
	if (!m_imGuiActive)
	{
		const int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
		m_camera.mouseEvents(glm::vec2(xPos, yPos), state == GLFW_PRESS);
	}
}

void MainWindow::mouseButtonCallback(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
	{
		double x, y;
		glfwGetCursorPos(m_window, &x, &y);

		performSelection(static_cast<int>(x), static_cast<int>(y));
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT)
	{
		performAddCube();
	}
}
