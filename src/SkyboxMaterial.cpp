/**
 * @file SkyboxMaterial.cpp
 *
 * @brief Material that takes lighting into account.
 *
 * Martin Johnson
 * Billy-Joe Lacasse
 * William Lebel
 */

#include "SkyboxMaterial.h"
#include "stb_image.h"

#include <glad/glad.h>
#include <iostream>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void SkyboxMaterial::bind() const
{
    glBindVertexArray(m_VAOs[VAO_Sphere_Sky]);
	Material::bind();
}

GLint SkyboxMaterial::uvAttribLocation() const
{
    return m_vUVLocation;
}

GLint SkyboxMaterial::tangentAttribLocation() const
{
    return GLint(- 1);
}


GLint SkyboxMaterial::positionAttribLocation() const
{
	return m_vPositionLocation;
}

GLint SkyboxMaterial::normalAttribLocation() const
{
	return m_vNormalLocation;
}

void SkyboxMaterial::setProjMatrix(const glm::mat4 &projMatrix) const
{
    m_shaderProgram->setMat4(projMatrixAttributeName, projMatrix);
}

void SkyboxMaterial::setViewRotMatrix(const glm::mat3 &viewMatrix) const
{
    m_shaderProgram->setMat3(viewRotMatrixAttributeName, viewMatrix);
}

void SkyboxMaterial::setSunPos(const glm::vec3& sunPos) const
{
    m_shaderProgram->setVec3(sunPosAttributeName, sunPos);
}

void SkyboxMaterial::setSunRot(const glm::vec2& sunRot) const
{
    m_shaderProgram->setVec2(sunPosAttributeName, sunRot);
}

void SkyboxMaterial::setViewDir(const glm::vec3& viewDir) const
{
    m_shaderProgram->setVec3(viewDirAttributeName, viewDir);
}

void SkyboxMaterial::loadTexture(unsigned int& textureId, const std::string& fileName){
    std::string asset_dir = ASSETS_DIR;

    glGenTextures(1,&textureId);

    stbi_set_flip_vertically_on_load(true);


    int width, height, nrChannels;
    std::string pathToAsset = asset_dir + fileName;
    unsigned char *data = stbi_load(&pathToAsset[0], &width, &height, &nrChannels, 0);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D,0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "Texture loaded at fileName: " << fileName << std::endl;
    }
    else {
        std::cout << "Failed to load at fileName : " << fileName << std::endl;
        stbi_image_free(data);
    }
}

void SkyboxMaterial::bindTextureAndDraw() const{
    glUniform1i(m_shaderProgram->uniformAttributeLocation(skyDomeTex1AttributeName),0);
    glUniform1i(m_shaderProgram->uniformAttributeLocation(skyDomeTex2AttributeName),1);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, TextureIdForward);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, TextureIdBackward);

    glDrawElements(GL_TRIANGLES, numTriSphere * 3, GL_UNSIGNED_INT, 0);
}

bool SkyboxMaterial::init_impl()
{
	if ((m_vPositionLocation = m_shaderProgram->attributeLocation(vPositionAttributeName)) < 0) {
		std::cerr << "Unable to find shader location for " << vPositionAttributeName << std::endl;
		return false;
	}

    glGenVertexArrays(NumVAOs, m_VAOs);
    glGenBuffers(NumBuffers, m_buffers);

    initGeometrySphere();

    loadTexture(TextureIdForward, "sky_color_forward.png");
    m_shaderProgram->setInt(skyDomeTex1AttributeName, 0);
    loadTexture(TextureIdBackward, "sky_color_backward.png");
    m_shaderProgram->setInt(skyDomeTex2AttributeName, 1);

	return true;
}

void SkyboxMaterial::initGeometrySphere()
{
    // Generate a sphere between [-0.5, 0.5]x[-0.5, 0.5]x[-0.5, 0.5]

    //Note: To ease the sphere creation, we use an index(aka elements) buffer.This allows us to create
    //			 each vertex once. Afterward, faces are created by specifying the index of the three vertices
    //			 inside the index buffer. For example, a 2D rectangle could be drawn using the following vertex and
    //       index buffers:
    //
    //	     vertices[4][2] = {{-1,-1},{1,-1},{1,1},{-1,1}};
    //       indices[2*3] = {0, 1, 3, 1, 2, 3};
    //
    //       In this example, the vertex buffer contains 4 vertices, and the index buffer contains two
    //       triangles formed by the vertices (vertices[0], vertices[1], vertices[3]) and (vertices[1],
    //       vertices[2], vertices[3]) respectively.
    //
    //       Also note that indices are stored in a different type of buffer called Element Array Buffer.

    // Create sphere vertices and faces
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLuint> indices;


    // Generate surrounding vertices
    float thetaInc = 2.0f * 3.14159265f / static_cast<float>(numColSphere);
    float phiInc = 3.14159265f / static_cast<float>(numRowSphere + 1); // note the +1 here
    const float radius = 0.9;
    for (int row = 0; row < numRowSphere; ++row)
    {
        // Phi ranges from Pi - Pi * 1 / (numRowSphere+1) to Pi - Pi * (numRowSphere) / (numRowSphere + 1)
        // Which is equivalent to :
        // Pi * numRowSphere/(numRowSphere+1) to Pi * 1/numRowSphere
        // You can think of Phi as sweeping the sphere from the South pole to the North pole
        float phi = 3.14159265f - (static_cast<float>(row + 1) * phiInc);
        for (int col = 0; col < numColSphere; ++col)
        {
            // Theta ranges from 0 to 2*Pi (numColSphere-1)/numColSphere
            // You can think of Theta as circling around the sphere, East to West
            float theta = col * thetaInc;

            // Spherical coordinates
            glm::vec3 pos = glm::vec3(radius * sin(theta) * sin(phi), radius * cos(phi), radius * cos(theta) * sin(phi));
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);

            normals.push_back(pos.x);
            normals.push_back(pos.y);
            normals.push_back(pos.z);
        }
    }

    // Generate cap vertices
    vertices.push_back(0.0f);
    vertices.push_back(-radius);
    vertices.push_back(0.0f);

    normals.push_back(0.0f);
    normals.push_back(-1.0f);
    normals.push_back(0.0f);

    vertices.push_back(0.0f);
    vertices.push_back(radius);
    vertices.push_back(0.0f);

    normals.push_back(0.0f);
    normals.push_back(1.0f);
    normals.push_back(0.0f);


    // Generate surrounding indices (faces)
    for (int row = 0; row < numRowSphere - 1; ++row)
    {
        unsigned int rowStart = row * numColSphere;
        unsigned int topRowStart = rowStart + numColSphere;

        for (int col = 0; col < numColSphere; ++col)
        {
            // Compute quad vertices
            unsigned int v = rowStart + col;
            unsigned int vi = (col < numColSphere - 1) ? v + 1 : rowStart;
            unsigned int vj = topRowStart + col;
            unsigned int vji = (col < numColSphere - 1) ? vj + 1 : topRowStart;

            // Add to indices
            indices.push_back(v);
            indices.push_back(vi);
            indices.push_back(vj);
            indices.push_back(vi);
            indices.push_back(vji);
            indices.push_back(vj);
        }
    }

    // Generate cap indices (faces)
    for (int col = 0; col < numColSphere; ++col)
    {
        indices.push_back(numColSphere * numRowSphere);
        indices.push_back((col < numColSphere - 1) ? col + 1 : 0);
        indices.push_back(col);

        unsigned int rowStart = (numRowSphere - 1) * numColSphere;
        indices.push_back(numColSphere * numRowSphere + 1);
        indices.push_back(rowStart + col);
        indices.push_back((col < numColSphere - 1) ? (rowStart + col + 1) : rowStart);
    }


    // Copy indices informations
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Sphere_Position]);
    glBufferData(GL_ARRAY_BUFFER, long(sizeof(GLfloat) * vertices.size()), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Sphere_Normal]);
    glBufferData(GL_ARRAY_BUFFER, long(sizeof(GLfloat) * normals.size()), normals.data(), GL_STATIC_DRAW);


    // Fill in indices EBO
    // Note: The current VAO will remember the call to glBindBuffer for a GL_ELEMENT_ARRAY_BUFFER.
    //			 However, we will need to call glDrawElements() instead of glDrawArrays().
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_Sphere]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // VAO for the skybox
    glBindVertexArray(m_VAOs[VAO_Sphere_Sky]);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Sphere_Position]);
    glVertexAttribPointer(m_vPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(m_vPositionLocation);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_Sphere]);

    // VAO for the normal shader
    glBindVertexArray(m_VAOs[VAO_Sphere]);
    // - Position
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Sphere_Position]);
    // - Normals
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VBO_Sphere_Normal]);
    // - Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EBO_Sphere]);

    // Cleanup (optional)

    // Do not desactivate EBO when the VAO is still activated
    // as it will desactivate the EBO for this VAO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}