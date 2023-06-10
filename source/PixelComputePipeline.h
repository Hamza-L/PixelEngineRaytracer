//
// Created by hlahm on 2023-05-31.
//

#ifndef PIXELENGINE_PIXELCOMPUTEPIPELINE_H
#define PIXELENGINE_PIXELCOMPUTEPIPELINE_H

#include "PixelImage.h"
#include "glm/glm.hpp"

class PixelComputePipeline {
public:
    PixelComputePipeline(PixBackend* backend, VkExtent2D inputExtent);
    PixelComputePipeline() = default;

    struct PObj{
        glm::vec3 cameraPos;
        float fov;
        glm::vec3 randomOffsets;
        float focus;
        glm::vec3 lightPos;
        float intensity;
        glm::vec4 lightColor;
        uint32_t currentSample;
        uint32_t mouseCoordX;
        uint32_t mouseCoordY;
        uint32_t outlineEnabled;
    };

    void addComputeShader(const std::string& filename);
    void createDescriptorPool();
    void createDescriptorSets();
    void initImageBufferStorage();
    void populatePipelineLayout();
    void createDescriptorSetLayout();
    void createComputePipeline();
    void createComputePipelineLayout();
    void init();
    void cleanUp();
    static constexpr VkPushConstantRange pushComputeConstantRange {VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PObj)};

    //getters
    VkPipeline getPipeline();
    VkPipelineLayout getPipelineLayout();
    VkDescriptorSet getDescriptorSet();
    PixelImage* getInputTexture();
    PixelImage* getOutputTexture();
    PixelImage* getCustomTexture();
    PObj* getPushObj(){return &test;}

    //setters
    void setPushObj(PixelComputePipeline::PObj pObj){test = pObj;}

private:

    VkExtent2D m_extent{};
    PixelImage raytracedInputTexture;
    PixelImage raytracedOutputTexture;
    PixelImage customTexture;

    PObj test = {{0.0f,1.0f,5.0f},35.0f,{0.0f,0.0f,0.0f},0.0f, {3.0f,4.0f,0.0f},0.0f,{1.0f,1.0f,1.0f,1.0f}, 0, 0, 0, 0};

    PixBackend* m_backend{};
    VkPipelineShaderStageCreateInfo computeCreateShaderInfo{};
    VkPipeline computePipeline = VK_NULL_HANDLE;
    VkPipelineLayout computePipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo computePipelineLayoutCreateInfo = {};
    VkShaderModule computeShaderModule = VK_NULL_HANDLE;
    VkDescriptorSetLayout computeDescriptorSetLayout{};
    VkDescriptorSet computeDescriptorSet{};
    VkDescriptorPool computeDescriptorPool{};
};


#endif //PIXELENGINE_PIXELCOMPUTEPIPELINE_H
