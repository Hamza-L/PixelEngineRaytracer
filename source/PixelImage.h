//
// Created by Zara Hussain on 2023-04-27.
//

#ifndef PIXELENGINE_PIXELIMAGE_H
#define PIXELENGINE_PIXELIMAGE_H

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN //includes vulkan automatically
#include <GLFW/glfw3.h>

#include "stb_image.h"
#include "Utility.h"

#include <iostream>

class PixelImage {
public:
    PixelImage(PixBackend* devices, uint32_t width, uint32_t height, bool isSwapChainImage);
    PixelImage() = default;

    //cleanup
    void cleanUp();

    //create functions
    void createImage(VkImageTiling imageTiling, VkImageUsageFlags useFlags, VkMemoryPropertyFlags propFlags);
    void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
    void createDepthBufferImage();
    void createTexture(std::string fileName);

    //setter functions
    void setName(std::string name);
    void setImage(VkImage inputImage){ m_image = inputImage;}
    void setImageView(VkImageView inputImageView){ m_imageView = inputImageView;}

    //getter functions
    std::string getName();
    uint32_t getWidth(){return m_width;}
    uint32_t getHeight(){return m_height;}
    VkImage getImage() { return m_image;}
    VkImageView getImageView() {return m_imageView;}
    VkDeviceMemory getImageDeviceMemory() {return m_imageMemory;}
    VkFormat getFormat();
    VkDeviceSize getImageBufferSize(){return m_imageSize;}
    stbi_uc* getImageData(){return m_imageData;}
    bool hasBeenInitialized(){return m_ImageInitialized;}
    bool hasBeenCleaned(){return m_ressourcesCleaned;}

    //helper functions

    //loader functions
    void loadTexture(std::string filename);
    void loadEmptyTexture();
    void loadEmptyTexture(uint32_t width, uint32_t height, VkImageUsageFlags flags);

private:

    //image data
    stbi_uc* m_imageData = nullptr;

    //image info
    uint32_t m_width{};
    uint32_t m_height{};
    std::string imageName{};
    bool m_IsSwapChainImage = false;
    bool m_ImageInitialized = false;
    bool m_ressourcesCleaned = false;

    //vulkan components
    PixBackend* m_device;
    VkFormat m_format{};
    VkDeviceSize m_imageSize{};
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE; //only applicable if PixelImage is used for depth buffer
};


#endif //PIXELENGINE_PIXELIMAGE_H
