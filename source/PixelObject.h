//
// Created by Zara Hussain on 2023-04-14.
//

#ifndef PIXELENGINE_PIXELOBJECT_H
#define PIXELENGINE_PIXELOBJECT_H

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/material.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "PixelImage.h"

#include <string>
#include <array>
#include <vector>

class PixelObject {
public:

    // this should noe change every frame, but can change per individual object/mesh.
    //Dynamic Uniform Buffer Object
    struct DynamicUBObj{
        glm::mat4 M{};
        glm::mat4 MinvT{};
        int texIndex = -1;
    };

    // this can change every frame, and can change per individual object/mesh.
    //Dynamic Uniform Buffer Object
    struct PObj{
        glm::mat4 M{};
        glm::mat4 MinvT{};
    };

    //the vertex must only contain member variables of type vec4 (each 16 bytes)
    struct Vertex
    {
        glm::vec4 position{};
        glm::vec4 normal{};
        glm::vec4 color{};
        glm::vec2 texUV{};
    };

    enum vertexAttributes
    {
        POSITION_ATTRIBUTEINDEX,
        NORMAL_ATTRIBUTEINDEX,
        COLOR_ATTRIBUTEINDEX,
        TEXUV_ATTRIBUTEINDEX,
        ATTRIBUTECOUNT
    };


    PixelObject(PixBackend* device, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    PixelObject(PixBackend* device, std::string filename);

    //getters
    int getVertexCount();
    std::vector<Vertex>* getVertices();
    VkDeviceSize getVertexBufferSize();
    VkBuffer* getVertexBuffer();
    VkDeviceMemory* getVertexBufferMemory();
    int getIndexCount();
    std::vector<uint32_t>* getIndices();
    VkDeviceSize getIndexBufferSize();
    VkBuffer* getIndexBuffer();
    VkDeviceMemory* getIndexBufferMemory();
    PObj* getPushObj();
    DynamicUBObj* getDynamicUBObj();
    std::vector<PixelImage> getTextures(){return m_textures;}
    int getGraphicsPipelineIndex(){return graphicsPipelineIndex;};
    static constexpr VkPushConstantRange pushConstantRange {VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PObj)};

    //setters
    void setDynamicUBObj(DynamicUBObj pushObjData);
    void setTexID(int texID){dynamicUBO.texIndex = texID;};
    void setPushObj(PObj pushObjData);
    void setGraphicsPipelineIndex(int pipelineIndx){graphicsPipelineIndex = pipelineIndx;};

    //cleanup
    void cleanup();


    //helper functions
    //returns the number of members of the Vertex Struct
    void importFile(const std::string& filename);
    void setGenericColor(glm::vec4 color);
    void addTransform(glm::mat4 matTransform);
    void setTransform(glm::mat4 matTransform);
    void addTexture(std::string textureFile);
    void addTexture(PixelImage* pixImage);
    void setTextureIDOffset(int offset){texIDOffset = offset;};
    void hide(){m_isHidden = true;};
    void unhide(){m_isHidden = false;};
    bool isHidden(){return m_isHidden;};


private:
    //member variables
    std::vector<Vertex> m_vertices{};
    std::vector<uint32_t> m_indices{};
    std::string name{};
    bool m_isHidden = false;

    //transforms
    DynamicUBObj dynamicUBO = {};
    PObj pushObj = {glm::mat4(1.0f)};

    //vulkan components
    PixBackend* m_device = VK_NULL_HANDLE;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

    //texture used
    std::vector<PixelImage> m_textures;
    int texIDOffset = 0;

    //pipeline used
    int graphicsPipelineIndex;
};


#endif //PIXELENGINE_PIXELOBJECT_H
