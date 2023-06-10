//
// Created by Zara Hussain on 2023-04-20.
//

#ifndef PIXELENGINE_PIXELSCENE_H
#define PIXELENGINE_PIXELSCENE_H

#define GLM_ENABLE_EXPERIMENTAL

#include "PixelObject.h"


static const glm::mat4 MAT4_IDENTITY = {1,0,0,0,
                                        0,1,0,0,
                                        0,0,1,0,
                                        0,0,0,1};

const int MAX_OBJECTS = 10;
const int MAX_TEXTURE_PER_OBJECT = 16;
enum DescSetLayoutIndex{
    UBOS,
    TEXTURES
};

class PixelScene {
public:
    PixelScene(VkDevice device, VkPhysicalDevice physicalDevice);
    //PixelScene(const PixelScene&) = delete;

    struct UboVP{
        glm::mat4 V = glm::mat4(1.0f);
        glm::mat4 P = glm::mat4(1.0f);
        glm::vec4 lightPos = glm::vec4(0.0f);
    };

    //setter functions
    void addObject(PixelObject pixObject);

    //getter functions
    VkDescriptorSetLayout* getDescriptorSetLayout(DescSetLayoutIndex indx);
    std::vector<VkDescriptorSetLayout>* getAllDescriptorSetLayouts();
    VkDescriptorPool* getDescriptorPool();
    VkDescriptorSet* getUniformDescriptorSetAt(int index);
    VkDescriptorSet* getTextureDescriptorSet();
    std::vector<VkDescriptorSet>* getUniformDescriptorSets();
    static VkDeviceSize getUniformBufferSize();
    VkDeviceSize getDynamicUniformBufferSize() const;
    VkDeviceSize getMinAlignment() const;
    VkBuffer* getUniformBuffers(int index);
    VkDeviceMemory* getUniformBufferMemories(int index);
    VkBuffer* getDynamicUniformBuffers(int index);
    VkDeviceMemory* getDynamicUniformBufferMemories(int index);
    int getNumObjects();
    PixelObject* getObjectAt(int index);
    std::vector<PixelImage> getAllTextures();
    UboVP getSceneVP();

    //setter functions
    void setSceneVP(UboVP vpData);
    void setSceneV(glm::mat4 V);
    void setSceneP(glm::mat4 P);

    //create functions
    void createDescriptorSetLayout();

    //update functons
    void updateUniformBuffer(uint32_t bufferIndex);
    void updateDynamicUniformBuffer(uint32_t bufferIndex);

    //helper functions
    void initialize();
    void resizeBuffers(size_t newSize);
    void resizeDesciptorSets(size_t newSize);
    static bool areMatricesEqual(glm::mat4 x, glm::mat4 y);

    //cleanup
    void cleanup();

private:

    //objects
    std::vector<PixelObject> allObjects{};
    std::vector<PixelObject::Vertex> allVertices{};
    std::vector<uint32_t> allIndices{};

    //helper functions
    void getMinUBOOffset(VkPhysicalDevice physicalDevice);

    //allocator functions
    void allocateDynamicBufferTransferSpace();

    //------UNIFORM BUFFER
    UboVP sceneVP; //model view projection matrix
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBufferMemories;
    std::vector<VkBuffer> dynamicUniformBuffers;
    std::vector<VkDeviceMemory> dynamicUniformBufferMemories;
    PixelObject::DynamicUBObj* modelTransferSpace{};

    //------TEXTURES

    //utility (Dynamic Buffers)
    VkDeviceSize minUBOOffset{}; //this is device specific and is a constant
    size_t objectUBOAllignment{}; //this is a multiple of minUBOOffset and will depend on the size of the object's UBO
    std::vector<bool> buffersUpdated;

    //vulkan component
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_uniformDescriptorSets{};
    std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts{};
    VkDescriptorSet m_textureDescriptorSet = VK_NULL_HANDLE;
};


#endif //PIXELENGINE_PIXELSCENE_H
