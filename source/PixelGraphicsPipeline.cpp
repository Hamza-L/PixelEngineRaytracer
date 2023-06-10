//
// Created by hlahm on 2023-04-01.
//

#include "PixelGraphicsPipeline.h"

#include <array>

void PixelGraphicsPipeline::addVertexShader(const std::string &filename) {
    vertexShaderModule = addShaderModule(m_device, filename);
}

void PixelGraphicsPipeline::addFragmentShader(const std::string &filename) {
    fragmentShaderModule = addShaderModule(m_device, filename);
}

void PixelGraphicsPipeline::createGraphicsPipeline(const VkRenderPass& inputRenderPass) {

    //the shader create infos have to be passed in as an array
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexCreateShaderInfo, fragmentCreateShaderInfo};

    VkResult result = vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    //Depth stencil testing
    //TODO:Setup Depth Stencil

    //create renderpass
    if(inputRenderPass == VK_NULL_HANDLE)
    {
        createRenderPass();
    } else
    {
        renderPass = inputRenderPass;
    }


    //graphics pipeline info
    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2; //vertex and fragment stage
    graphicsPipelineCreateInfo.pStages = shaderStages; //list of shader stages
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo; //all fixed functions pipeline stages
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = VK_NULL_HANDLE; //&dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &blendStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = renderPassDepthAttachment.hasBeenDefined ? &depthStencilStateCreateInfo : nullptr;
    graphicsPipelineCreateInfo.layout = pipelineLayout; //pipeline layout this pipeline should use
    graphicsPipelineCreateInfo.renderPass = renderPass; //render pass description the pipeline is compatible with
    graphicsPipelineCreateInfo.subpass = 0; //one subpass per pipeline

    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; //existing pipeline to derive from
    graphicsPipelineCreateInfo.basePipelineIndex = -1; //or index pipeline from of multiple pipelines created once suing specfic funciton

    //create graphics pipeline
    result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline");
    }

    vkDestroyShaderModule(m_device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(m_device, fragmentShaderModule, nullptr);
}

void PixelGraphicsPipeline::cleanUp() {
    vkDestroyPipeline(m_device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_device, pipelineLayout, nullptr);

    if(renderPass != VK_NULL_HANDLE && wasRenderPassCreated)
    vkDestroyRenderPass(m_device, renderPass, nullptr);
}

void PixelGraphicsPipeline::createRenderPass() {

    //ATTACHMENTS
    std::vector<VkAttachmentReference> colorAttachmentReferences; //Only the color attachments
    std::vector<VkAttachmentDescription> allAttachmentDescriptions; //color attachments and the depth attachment

    for(size_t i = 0; i < renderPassColorAttachments.size(); i++)
    {
        colorAttachmentReferences.push_back(renderPassColorAttachments[i].attachmentReference);
        allAttachmentDescriptions.push_back(renderPassColorAttachments[i].attachmentDescription);
    }

    //info about our first subpass
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = static_cast<uint32_t>(renderPassColorAttachments.size());
    subpassDescription.pColorAttachments = colorAttachmentReferences.data();
    if(renderPassDepthAttachment.hasBeenDefined)
    {
        subpassDescription.pDepthStencilAttachment = &renderPassDepthAttachment.attachmentReference;
        allAttachmentDescriptions.push_back(renderPassDepthAttachment.attachmentDescription);
    }

    //dependencies
    std::array<VkSubpassDependency, 2> subpassDependencies = {};
    //conversion from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    //SUBPASS MUST HAPPEN AFTER THE FOLLOWING
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL; //anything that takes place outside of the subpass
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; //the end of the pipeline. This stage has to happen first before we proceed to this subpass
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    //SUBPASS MUST HAPPEN BEFORE THE FOLLOWING
    subpassDependencies[0].dstSubpass = 0; //the first subpass in the list we sent
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //this stage has to happen after this subpass;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = 0; //no dependency flags

    //conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    //SUBPASS MUST HAPPEN AFTER THE FOLLOWING
    subpassDependencies[1].srcSubpass = 0; //anything that takes place outside of the subpass
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; //the end of the pipeline. This stage has to happen first before we proceed to this subpass
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    //SUBPASS MUST HAPPEN BEFORE THE FOLLOWING
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; //the first subpass in the list we sent
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; //this stage has to happen after this subpass;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags = 0; //no dependency flags

    //create info for renderpass
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(allAttachmentDescriptions.size());
    renderPassCreateInfo.pAttachments = allAttachmentDescriptions.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
    renderPassCreateInfo.pDependencies = subpassDependencies.data();

    VkResult result = vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &renderPass);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create renderpass");
    } else
    {
        wasRenderPassCreated = true;
    }
}

PixelGraphicsPipeline::PixelGraphicsPipeline(VkDevice device, VkExtent2D inputExtent) : m_device(device), extent(inputExtent) {

}

VkRenderPass PixelGraphicsPipeline::getRenderPass() {
    return renderPass;
}

void PixelGraphicsPipeline::populateGraphicsPipelineInfo() {
    //Vertex-Stage creation
    vertexCreateShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexCreateShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexCreateShaderInfo.module = vertexShaderModule;
    vertexCreateShaderInfo.pName = "main"; //the entry point of the shader

    //Vertex-Stage creation
    fragmentCreateShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentCreateShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentCreateShaderInfo.module = fragmentShaderModule;
    fragmentCreateShaderInfo.pName = "main"; //the entry point of the shader

    //vertex input info-----------
    //How data for a single vertex is laid out

    inputBindingDescription.binding = 0;
    inputBindingDescription.stride = sizeof(PixelObject::Vertex);
    inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //how to move between data after each vertex
                                                                     //VK_VERTEX_INPUT_RATE_VERTEX : Move on to the next vertex
                                                                     //VK_VERTEX_INPUT_RATE_INSTANCE : Move on to the next instance

    //How the data within a vertex is descripted
    //fills in each Vertex Input Attribute Description struct for each attributes in the Vertex Object (position, color etc...)
    inputAttributeDescription[PixelObject::POSITION_ATTRIBUTEINDEX].binding = 0; //matches the layout(binding = 0)
    inputAttributeDescription[PixelObject::POSITION_ATTRIBUTEINDEX].location = static_cast<uint32_t>(PixelObject::POSITION_ATTRIBUTEINDEX); //matches the layout(location = 0)
    inputAttributeDescription[PixelObject::POSITION_ATTRIBUTEINDEX].format = VK_FORMAT_R32G32B32A32_SFLOAT; //the format of the attribute (vec3)
    inputAttributeDescription[PixelObject::POSITION_ATTRIBUTEINDEX].offset = static_cast<uint32_t>(offsetof(PixelObject::Vertex, position)); //each vec4 has 16 bytes. so the offset into the struct shifts by 16 bytes per vec4

    inputAttributeDescription[PixelObject::NORMAL_ATTRIBUTEINDEX].binding = 0; //matches the layout(binding = 0)
    inputAttributeDescription[PixelObject::NORMAL_ATTRIBUTEINDEX].location = static_cast<uint32_t>(PixelObject::NORMAL_ATTRIBUTEINDEX); //matches the layout(location = 0)
    inputAttributeDescription[PixelObject::NORMAL_ATTRIBUTEINDEX].format = VK_FORMAT_R32G32B32A32_SFLOAT; //the format of the attribute (vec3)
    inputAttributeDescription[PixelObject::NORMAL_ATTRIBUTEINDEX].offset = static_cast<uint32_t>(offsetof(PixelObject::Vertex, normal)); //each vec4 has 16 bytes. so the offset into the struct shifts by 16 bytes per vec4

    inputAttributeDescription[PixelObject::COLOR_ATTRIBUTEINDEX].binding = 0; //matches the layout(binding = 0)
    inputAttributeDescription[PixelObject::COLOR_ATTRIBUTEINDEX].location = static_cast<uint32_t>(PixelObject::COLOR_ATTRIBUTEINDEX); //matches the layout(location = 0)
    inputAttributeDescription[PixelObject::COLOR_ATTRIBUTEINDEX].format = VK_FORMAT_R32G32B32A32_SFLOAT; //the format of the attribute (vec3)
    inputAttributeDescription[PixelObject::COLOR_ATTRIBUTEINDEX].offset = static_cast<uint32_t>(offsetof(PixelObject::Vertex, color)); //each vec4 has 16 bytes. so the offset into the struct shifts by 16 bytes per vec4

    inputAttributeDescription[PixelObject::TEXUV_ATTRIBUTEINDEX].binding = 0; //matches the layout(binding = 0)
    inputAttributeDescription[PixelObject::TEXUV_ATTRIBUTEINDEX].location = static_cast<uint32_t>(PixelObject::TEXUV_ATTRIBUTEINDEX); //matches the layout(location = 0)
    inputAttributeDescription[PixelObject::TEXUV_ATTRIBUTEINDEX].format = VK_FORMAT_R32G32_SFLOAT; //the format of the attribute (vec3)
    inputAttributeDescription[PixelObject::TEXUV_ATTRIBUTEINDEX].offset = static_cast<uint32_t>(offsetof(PixelObject::Vertex, texUV)); //each vec4 has 16 bytes. so the offset into the struct shifts by 16 bytes per vec4

    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &inputBindingDescription; //list of binding description info (spacing, stride etc,,,)
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(PixelObject::ATTRIBUTECOUNT);
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = inputAttributeDescription.data(); //list of attribute description (data format and where to bind to/from)

    //Input Assembly
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = false; // allow to restart list of triangle fans

    //viewport and scissors
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    scissor.offset = {0,0};
    scissor.extent = extent;
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    //dynamic state
    /*
    //point to somethings you may change dynamically
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicstates.size());
    dynamicStateCreateInfo.pDynamicStates = dynamicstates.data();
     */

    // depth stencil create info
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE; //is it using the depth stencil buffer for depth testing
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE; //can we update the depth pixel of our depth buffer
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS; //so if the depth value of the current pixel is less, we overwrite.
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE; //we can check if it is between two values;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE; //we do not do any stencil test

    //rasterizer
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE; //change if fragments beyond the far plane are clipped to plane. requires GPU Feature
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE; //suitable for pipeline without framebuffer
    rasterizationStateCreateInfo.lineWidth = 1.0f; //need gpu feature for anything else then 1
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;


    //multisampling
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //blending
    blendAttachmentState.colorWriteMask =   VK_COLOR_COMPONENT_R_BIT |
                                            VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT |
                                            VK_COLOR_COMPONENT_A_BIT;
    blendAttachmentState.blendEnable = VK_TRUE;
    blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    //blending uses the following equation
    //----- (srcColorBlendFactor * newColor) colorBlendOp (dstColorBlendFactor * oldColor) = result

    blendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendStateCreateInfo.logicOpEnable = VK_FALSE; //alternative to calculation is to use logic op
    blendStateCreateInfo.attachmentCount = 1;
    blendStateCreateInfo.pAttachments = &blendAttachmentState;

    //pipeline layout
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
}

VkPipeline PixelGraphicsPipeline::getPipeline() {
    return graphicsPipeline;
}

void PixelGraphicsPipeline::populatePipelineLayout(PixelScene* scene) {

    //pipeline layout
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(scene->getAllDescriptorSetLayouts()->size());
    pipelineLayoutCreateInfo.pSetLayouts = scene->getAllDescriptorSetLayouts()->data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &PixelObject::pushConstantRange;
}

VkPipelineLayout PixelGraphicsPipeline::getPipelineLayout() {
    return pipelineLayout;
}

void
PixelGraphicsPipeline::addRenderpassColorAttachment(VkFormat imageFormat, VkImageLayout initialLayout, VkImageLayout finalLayout, VkAttachmentStoreOp attachmentStoreOp,
                                               VkImageLayout attachmentReferenceLayout) {

    PixRenderpassAttachement attachment{};
    attachment.attachmentDescription.format = imageFormat;
    attachment.attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //this clears the buffer when we start the renderpass
    attachment.attachmentDescription.storeOp = attachmentStoreOp; //we want to present the result so we keep it
    attachment.attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.attachmentDescription.initialLayout = initialLayout; //renderpass (before the subpasses) layout, default = VK_IMAGE_LAYOUT_UNDEFINED
    attachment.attachmentDescription.finalLayout = finalLayout; //renderpass (after the subpasses) layout, default = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

    attachment.attachmentReference.attachment = renderPassColorAttachments.size();
    attachment.attachmentReference.layout = attachmentReferenceLayout; //default = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL

    attachment.hasBeenDefined = true;

    renderPassColorAttachments.push_back(attachment);

    //making sure the depth attachment, if defined before the color attachment, is indeed the last attachment index of the renderpass.
    renderPassDepthAttachment.attachmentReference.attachment = renderPassColorAttachments.size();
}

void PixelGraphicsPipeline::addRenderpassDepthAttachment(VkFormat depthImageFormat) {

    renderPassDepthAttachment.attachmentDescription.format = depthImageFormat;
    renderPassDepthAttachment.attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    renderPassDepthAttachment.attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //this clears the buffer when we start the renderpass
    renderPassDepthAttachment.attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //we want to present the result so we keep it
    renderPassDepthAttachment.attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    renderPassDepthAttachment.attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    renderPassDepthAttachment.attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //renderpass (before the subpasses) layout, default = VK_IMAGE_LAYOUT_UNDEFINED
    renderPassDepthAttachment.attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //renderpass (after the subpasses) layout, default = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

    renderPassDepthAttachment.attachmentReference.attachment = renderPassColorAttachments.size();
    renderPassDepthAttachment.attachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //default = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL

    renderPassDepthAttachment.hasBeenDefined = true;

}

void PixelGraphicsPipeline::setScreenDimensions(float x0, float x1, float y0, float y1) {

    viewport.x = x0;
    viewport.y = y0;
    viewport.width = x1-x0;
    viewport.height = y1-y0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    scissor.offset = {(int32_t)x0,(int32_t) y0};
    scissor.extent.width = x1-x0;
    scissor.extent.height = y1-y0;
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;
}

void PixelGraphicsPipeline::setPolygonMode(VkPolygonMode polygonMode) {
    rasterizationStateCreateInfo.polygonMode = polygonMode;
}


