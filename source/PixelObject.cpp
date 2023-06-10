//
// Created by Zara Hussain on 2023-04-14.
//

#include "PixelObject.h"

#include <utility>
#include <fstream>


PixelObject::PixelObject(PixBackend* device, std::vector<Vertex> vertices, std::vector<uint32_t> indices): m_device(device), m_vertices(std::move(vertices)), m_indices(std::move(indices)) {
    //create the vertex buffer form the vertices
    printf("PixelObject user constructed\n");
    //createVertexBuffer(vertices);
}

void PixelObject::cleanup() {

    for(auto texture : m_textures)
    {
        if(!texture.hasBeenCleaned())
        {
            texture.cleanUp();
        }
    }

    vkFreeMemory(m_device->logicalDevice, vertexBufferMemory, nullptr);
    vkDestroyBuffer(m_device->logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(m_device->logicalDevice, indexBufferMemory, nullptr);
    vkDestroyBuffer(m_device->logicalDevice, indexBuffer, nullptr);
}

std::vector<PixelObject::Vertex>* PixelObject::getVertices() {
    return &m_vertices;
}

int PixelObject::getVertexCount() {
    return m_vertices.size();
}

VkDeviceSize PixelObject::getVertexBufferSize() {
    return (sizeof(Vertex) * m_vertices.size());
}

VkBuffer* PixelObject::getVertexBuffer() {
    return &vertexBuffer;
}

VkDeviceMemory *PixelObject::getVertexBufferMemory() {
    return &vertexBufferMemory;
}

std::vector<uint32_t> *PixelObject::getIndices() {
    return &m_indices;
}

int PixelObject::getIndexCount() {
    return m_indices.size();
}

VkDeviceSize PixelObject::getIndexBufferSize() {
    return (sizeof(uint32_t) * m_indices.size());
}

VkBuffer *PixelObject::getIndexBuffer() {
    return &indexBuffer;
}

VkDeviceMemory *PixelObject::getIndexBufferMemory() {
    return &indexBufferMemory;
}

void PixelObject::setDynamicUBObj(DynamicUBObj pushObjData) {
    dynamicUBO = pushObjData;
}

PixelObject::PObj* PixelObject::getPushObj() {
    return &pushObj;
}

PixelObject::PixelObject(PixBackend *device, std::string filename) : m_device(device){
    importFile(filename);
}

void PixelObject::importFile(const std::string& filename) {



    enum Point {POSITION, TEXTURE, NORMAL};

    std::string fileLocation = "objects/" + filename;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile( fileLocation,
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate|
                                              aiProcess_FlipUVs);

    if (nullptr == scene) {
        throw std::runtime_error( importer.GetErrorString());
    }

    int vertCounter = 0;
    int indxCounter = 0;
    for(int m = 0; m < scene->mNumMeshes; m++)
    {
        vertCounter+=scene->mMeshes[m]->mNumVertices;
        indxCounter+= (scene->mMeshes[m]->mNumFaces * 3);
    }

    for(int m = 0; m < scene->mNumMeshes; m++)
    {
        for(int f = 0; f < scene->mMeshes[m]->mNumFaces; f++)
        {
            for(int i = 0; i < scene->mMeshes[m]->mFaces[f].mNumIndices; i++)
            {
                m_indices.push_back(m_indices.size());
            }
        }

        for(int i = 0; i<scene->mMeshes[m]->mNumVertices; i++)
        {
            Vertex v;
            v.position = glm::vec4(scene->mMeshes[m]->mVertices[i].x,
                                   scene->mMeshes[m]->mVertices[i].y,
                                   scene->mMeshes[m]->mVertices[i].z,1.0f);
            v.normal = glm::vec4(scene->mMeshes[m]->mNormals[i].x,
                                 scene->mMeshes[m]->mNormals[i].y,
                                 scene->mMeshes[m]->mNormals[i].z,0.0f);

            if(scene->mMeshes[m]->HasTextureCoords(0))
            {
                v.texUV = glm::vec2(scene->mMeshes[m]->mTextureCoords[0][i].x,
                                    scene->mMeshes[m]->mTextureCoords[0][i].y);
            }

            v.color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);

            m_vertices.push_back(v);
        }
    }


}

void PixelObject::addTransform(glm::mat4 matTransform) {
    pushObj.M = matTransform * pushObj.M;
    pushObj.MinvT = glm::transpose(glm::inverse(pushObj.M));
}

void PixelObject::setTransform(glm::mat4 matTransform) {
    pushObj.M = matTransform;
    pushObj.MinvT = glm::transpose(glm::inverse(pushObj.M));
}

void PixelObject::setPushObj(PixelObject::PObj pushObjData) {
    pushObj = PObj(pushObjData);
}

PixelObject::DynamicUBObj* PixelObject::getDynamicUBObj() {
    return &dynamicUBO;
}

void PixelObject::addTexture(std::string textureFile) {
    PixelImage textureImage = PixelImage(m_device, 0, 0, false);
    textureImage.loadTexture(textureFile);

    setTexID(0);

    m_textures.push_back(textureImage);

}

void PixelObject::addTexture(PixelImage* pixImage) {

    setTexID(0);

    m_textures.push_back(*pixImage);

}

