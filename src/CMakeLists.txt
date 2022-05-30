# Add source files
SET(SOURCE_FILES 
	Main.cpp Camera.cpp ShaderProgram.cpp MainWindow.cpp Material.cpp ConstantMaterial.cpp SceneObject.cpp Transform.cpp MeshRenderer.cpp Mesh.cpp CubeMesh.cpp OBJLoader.cpp TextureMaterial.cpp ObjectMesh.cpp SkyboxMaterial.cpp
)
set(HEADER_FILES 
	Camera.h MainWindow.h ShaderProgram.h Material.h ConstantMaterial.h SceneObject.h Transform.h MeshRenderer.h Mesh.h CubeMesh.h OBJLoader.h TextureMaterial.h ExtraOperators.h SkyboxMaterial.h
)
set(SHADER_FILES 
	constantShader.vert constantShader.frag textureShader.vert textureShader.frag skydome.frag skydome.vert
)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})

# Definition (SHADER_FILES)
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Define the link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})