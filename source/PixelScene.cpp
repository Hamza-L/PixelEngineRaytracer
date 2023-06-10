//
// Created by Zara Hussain on 2023-04-20.
//

#include "PixelScene.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_relational.hpp"

#include <vector>
#include <cstdlib>

PixelScene::PixelScene(VkDevice device, VkPhysicalDevice physicalDevice) : m_device(device), m_physicalDevice(physicalDevice)
{
    printf("PixelScene constructed\n");
    initialize();
}

void PixelScene::cleanup()
{

#ifdef __APPLE__
    free(modelTransferSpace);

#else
    _aligned_free(modelTransferSpace);
#endif


    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayouts[UBOS], nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayouts[TEXTURES], nullptr);
    for(int i = 0; i < uniformBuffers.size(); i++)
    {
        vkDestroyBuffer(m_device, dynamicUniformBuffers[i], nullptr);
        vkFreeMemory(m_device, dynamicUniformBufferMemories[i], nullptr);
        vkDestroyBuffer(m_device, uniformBuffers[i], nullptr);
        vkFreeMemory(m_device, uniformBufferMemories[i], nullptr);
    }

    for(auto& object : allObjects)
    {
        object.cleanup();
    }
}

VkDescriptorSetLayout* PixelScene::getDescriptorSetLayout(DescSetLayoutIndex indx) {

    if(indx == UBOS || indx == TEXTURES)
    {
        return &m_descriptorSetLayouts[indx];
    }

    return VK_NULL_HANDLE;
}

VkDeviceSize PixelScene::getUniformBufferSize() {
    return sizeof(UboVP);
}

VkBuffer* PixelScene::getUniformBuffers(int index) {
    return &(uniformBuffers[index]);
}

VkDeviceMemory* PixelScene::getUniformBufferMemories(int index) {
    return &(uniformBufferMemories[index]);
}

void PixelScene::resizeBuffers(size_t newSize) {
    uniformBuffers.resize(newSize);
    uniformBufferMemories.resize(newSize);
    dynamicUniformBuffers.resize(newSize);
    dynamicUniformBufferMemories.resize(newSize);
    buffersUpdated.resize(newSize, false);
}

void PixelScene::addObject(PixelObject pixObject) {
    if(pixObject.getTextures().size() > 0)
    {
        pixObject.setTextureIDOffset(getAllTextures().size());
    }
    allObjects.push_back(pixObject);
}

int PixelScene::getNumObjects() {
    return allObjects.size();
}

PixelObject* PixelScene::getObjectAt(int index) {
    return &allObjects[index];
}

VkDescriptorPool *PixelScene::getDescriptorPool() {
    return &m_descriptorPool;
}

VkDescriptorSet* PixelScene::getUniformDescriptorSetAt(int index) {
    return &m_uniformDescriptorSets[index];
}

void PixelScene::resizeDesciptorSets(size_t newSize) {
    m_uniformDescriptorSets.resize(newSize);
}

std::vector<VkDescriptorSet>* PixelScene::getUniformDescriptorSets() {
    return &m_uniformDescriptorSets;
}

void PixelScene::updateUniformBuffer(uint32_t bufferIndex)
{
    bool bufferNeedUpdate = false;
    //check if any buffer is expired and need updating
    for(bool hasBufferUpdated : buffersUpdated)
    {
        if(!hasBufferUpdated)
        {
            bufferNeedUpdate = true;
            break;
        }
    }

    if(bufferNeedUpdate)
    {
        sceneVP.P[1][1] *= -1; //invert the y scale to flip the image. Vulkan is flipped by default

        void* data;
        vkMapMemory(m_device, uniformBufferMemories[bufferIndex], 0, getUniformBufferSize(),0,&data);
        memcpy(data, &sceneVP, getUniformBufferSize());
        vkUnmapMemory(m_device, uniformBufferMemories[bufferIndex]);

        buffersUpdated[bufferIndex] = true;
    }

}

void PixelScene::createDescriptorSetLayout() {

    VkDescriptorSetLayout uniformDescriptorSetLayout{};
    VkDescriptorSetLayout textureDescriptorSetLayout{};

    //how data is bound to the shader in binding 0
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{};
    uniformBufferLayoutBinding.binding = 0; //binding point in shader
    uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayoutBinding.descriptorCount = 1; //only binding one uniform buffer
    uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uniformBufferLayoutBinding.pImmutableSamplers = nullptr;

    //how data is bound to the shader in binding 1
    VkDescriptorSetLayoutBinding dynamicBufferLayoutBinding{};
    dynamicBufferLayoutBinding.binding = 1; //binding point in shader
    dynamicBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    dynamicBufferLayoutBinding.descriptorCount = 1; //only binding one uniform buffer
    dynamicBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    dynamicBufferLayoutBinding.pImmutableSamplers = nullptr;

    //how data is bound to the shader in binding 2
    VkDescriptorSetLayoutBinding textureSamplerLayoutBinding{};
    textureSamplerLayoutBinding.binding = 0; //binding point in shader
    textureSamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureSamplerLayoutBinding.descriptorCount = static_cast<uint32_t>(MAX_TEXTURE_PER_OBJECT); //only binding one combine image sampler buffer
    textureSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureSamplerLayoutBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> descriptorSetLayoutBindings = {uniformBufferLayoutBinding, dynamicBufferLayoutBinding};

    //Create descriptor set layout given binding
    VkDescriptorSetLayoutCreateInfo uniformBufferObjectDescriptorSetlayoutCreateInfo{};
    uniformBufferObjectDescriptorSetlayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    uniformBufferObjectDescriptorSetlayoutCreateInfo.pBindings = descriptorSetLayoutBindings.data();
    uniformBufferObjectDescriptorSetlayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());

    VkResult result = vkCreateDescriptorSetLayout(m_device, &uniformBufferObjectDescriptorSetlayoutCreateInfo, nullptr, &uniformDescriptorSetLayout);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout for ubos");
    }

    //Create descriptor set layout given binding
    VkDescriptorSetLayoutCreateInfo textureDescriptorSetLayoutCreateInfo{};
    textureDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    textureDescriptorSetLayoutCreateInfo.pBindings = &textureSamplerLayoutBinding;
    textureDescriptorSetLayoutCreateInfo.bindingCount = 1;

    result = vkCreateDescriptorSetLayout(m_device, &textureDescriptorSetLayoutCreateInfo, nullptr, &textureDescriptorSetLayout);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor set layout for textures");
    }

    m_descriptorSetLayouts.push_back(uniformDescriptorSetLayout);
    m_descriptorSetLayouts.push_back(textureDescriptorSetLayout);
}

PixelScene::UboVP PixelScene::getSceneVP() {
    return sceneVP;
}

void PixelScene::setSceneVP(PixelScene::UboVP vpData)
{
    sceneVP = vpData;
}

void PixelScene::setSceneV(glm::mat4 V) {
    sceneVP.V = glm::mat4(V);
}

void PixelScene::setSceneP(glm::mat4 P) {
    sceneVP.P = glm::mat4(P);
}

bool PixelScene::areMatricesEqual(glm::mat4 x, glm::mat4 y) {

    for(int i = 0; i < 4 ; i++)
    {
        for(int j = 0; j < 4 ; j++)
        {
            if(abs(x[i][j] - y[i][j]) >= 0.000001f)
            {
                return false;
            }
        }
    }

    return true;
}

void PixelScene::allocateDynamicBufferTransferSpace()
{
    //calculate allignment
    objectUBOAllignment = (sizeof(PixelObject::DynamicUBObj) + minUBOOffset - 1) & ~(minUBOOffset - 1); //right portion is our mask

    
#ifdef __APPLE__
//create memory for all the objects dynamic buffers;
    modelTransferSpace = (PixelObject::DynamicUBObj*) aligned_alloc(minUBOOffset, objectUBOAllignment * MAX_OBJECTS);
#else
//create memory for all the objects dynamic buffers;
    modelTransferSpace = (PixelObject::DynamicUBObj*) _aligned_malloc(objectUBOAllignment * MAX_OBJECTS, minUBOOffset);
#endif

}

void PixelScene::getMinUBOOffset(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties physicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    minUBOOffset = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
}

VkBuffer *PixelScene::getDynamicUniformBuffers(int index) {
    return &(dynamicUniformBuffers[index]);
}

VkDeviceMemory *PixelScene::getDynamicUniformBufferMemories(int index) {
    return &(dynamicUniformBufferMemories[index]);
}

VkDeviceSize PixelScene::getDynamicUniformBufferSize() const {
    return objectUBOAllignment * MAX_OBJECTS;
}

VkDeviceSize PixelScene::getMinAlignment() const {
    return objectUBOAllignment;
}

void PixelScene::updateDynamicUniformBuffer(uint32_t bufferIndex) {
    for(size_t i = 0; i<allObjects.size(); i++)
    {
        auto* currentPushM = (PixelObject::DynamicUBObj*)((uint64_t)modelTransferSpace + (i * objectUBOAllignment));
        *currentPushM = *(allObjects[i].getDynamicUBObj());
    }

    //map the whole chunk of memory data
    void* data;
    vkMapMemory(m_device, dynamicUniformBufferMemories[bufferIndex], 0, objectUBOAllignment * allObjects.size() , 0 , &data);
    memcpy(data, modelTransferSpace, objectUBOAllignment * allObjects.size());
    vkUnmapMemory(m_device, dynamicUniformBufferMemories[bufferIndex]);
}

void PixelScene::initialize() {
    getMinUBOOffset(m_physicalDevice);
    allocateDynamicBufferTransferSpace();
    createDescriptorSetLayout();
}

std::vector<PixelImage> PixelScene::getAllTextures() {
    std::vector<PixelImage> allTextures;

    for(int i = 0 ; i < allObjects.size(); i++)
    {
        for(int j = 0; j < allObjects[i].getTextures().size(); j++)
        {
            allTextures.push_back(allObjects[i].getTextures()[j]);
        }
    }

    return allTextures;
}

std::vector<VkDescriptorSetLayout> *PixelScene::getAllDescriptorSetLayouts() {
    return &m_descriptorSetLayouts;
}

VkDescriptorSet *PixelScene::getTextureDescriptorSet() {
    return &m_textureDescriptorSet;
}
