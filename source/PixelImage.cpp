//
// Created by Zara Hussain on 2023-04-27.
//

#include "PixelImage.h"

PixelImage::PixelImage(PixBackend* device, uint32_t width, uint32_t height, bool isSwapChainImage) : m_device(device), m_width(width), m_height(height), m_IsSwapChainImage(isSwapChainImage) {
    if (m_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Logical device not created. current Image cannot be initialized");
    }
}

void PixelImage::cleanUp()
{
    if(m_device->logicalDevice == VK_NULL_HANDLE || m_device->physicalDevice == VK_NULL_HANDLE || m_imageView == VK_NULL_HANDLE)
    {
        return;
    }

    stbi_image_free(m_imageData);
    vkDestroyImageView(m_device->logicalDevice, m_imageView, nullptr);
    if(!m_IsSwapChainImage)
    {
        vkDestroyImage(m_device->logicalDevice, m_image, nullptr); //if swapchain image. it will be destroyed by the swapchain (just like it was created by the swapchain)
        vkFreeMemory(m_device->logicalDevice, m_imageMemory, nullptr);
    }

    m_ressourcesCleaned = true;
}

//create an image view for the image
void PixelImage::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
{

    m_format = format; //keep the format for reference.

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    //Subresource allow the view to view only a part of an image
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags; //which aspect of image to use (color bit for viewing color)
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0; //start mip map level to view from
    imageViewCreateInfo.subresourceRange.levelCount = 1; //levels of mip map to view
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0; //start array level to view from
    imageViewCreateInfo.subresourceRange.layerCount = 1; //layers to view

    //create image view and return it
    VkResult result = vkCreateImageView(m_device->logicalDevice, &imageViewCreateInfo, nullptr, &m_imageView);

    if (result != VK_SUCCESS)
    {
        std::string message = "Was not able to create image view for image: ";
        message.append(imageName);
        throw std::runtime_error(message.c_str());
    }

    m_ImageInitialized = true;
    m_ressourcesCleaned = false;
}

std::string PixelImage::getName()
{
    return imageName;
}

void PixelImage::setName(std::string name)
{
    imageName.clear();
    imageName = name;
}

void PixelImage::createDepthBufferImage()
{

    m_format = chooseSupportedFormat(m_device->physicalDevice,
                                     {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT}, //depth buffer at 32bit and stencil buffer at 8bit ideally
                                     VK_IMAGE_TILING_OPTIMAL,
                                      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    createImage(VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView(m_format, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void PixelImage::createImage(VkImageTiling imageTiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags) {

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.extent.width = m_width;
    imageCreateInfo.extent.height = m_height;
    imageCreateInfo.extent.depth = 1; //no 3D aspect
    imageCreateInfo.mipLevels = 1; //TODO: implement mipmap
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.format = m_format;
    imageCreateInfo.tiling = imageTiling;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //matches the renderpass's initial layout for the color attachment
    imageCreateInfo.usage = useFlags;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT; //TODO: implement multisampling
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //can it be shared between queues

    VkResult result = vkCreateImage(m_device->logicalDevice, &imageCreateInfo, nullptr, &m_image);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image");
    }

    VkMemoryRequirements imageMemoryRequirements;
    vkGetImageMemoryRequirements(m_device->logicalDevice, m_image, &imageMemoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo{};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(m_device->physicalDevice,
                                                             imageMemoryRequirements.memoryTypeBits,
                                                             propFlags);

    result = vkAllocateMemory(m_device->logicalDevice, &memoryAllocateInfo, nullptr, &m_imageMemory);
    if(result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate memory for image");
    }

    //connect image to memory
    vkBindImageMemory(m_device->logicalDevice, m_image, m_imageMemory, 0);
}

VkFormat PixelImage::getFormat() {
    return m_format;
}

void PixelImage::loadTexture(std::string filename) {

    int channels, width, height;

    std::string fileLocation = "Textures/" + filename;
    stbi_uc* image = stbi_load(fileLocation.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    m_width = (int)width;
    m_height = (int) height;

    if(!image)
    {
        throw std::runtime_error("Failed to load texture file: " + fileLocation);
    }

    m_imageSize = m_width * m_height * 4;
    m_imageData = image;

    //now that the image data and the information about the imagefile has been stored, we create the VkImage and the VkImageView for our texture
    m_format = VK_FORMAT_R8G8B8A8_UNORM; //here we set the format manually, we do not need to check if it is compatible with other features

    createImage(VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView(m_format, VK_IMAGE_ASPECT_COLOR_BIT);
}

void PixelImage::loadEmptyTexture() {
    m_width = 1;
    m_height = 1;
    m_imageSize = 1;
    m_format = VK_FORMAT_R8G8B8A8_UNORM; //here we set the format manually, we do not need to check if it is compatible with other features

    createImage(VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView(m_format, VK_IMAGE_ASPECT_COLOR_BIT);
}

void PixelImage::loadEmptyTexture(uint32_t width, uint32_t height, VkImageUsageFlags flags) {
    m_width = width;
    m_height = height;
    m_imageSize = width * height * 4;
    m_format = VK_FORMAT_R8G8B8A8_UNORM; //here we set the format manually, we do not need to check if it is compatible with other features

    createImage(VK_IMAGE_TILING_OPTIMAL, flags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    createImageView(m_format, VK_IMAGE_ASPECT_COLOR_BIT);
}
