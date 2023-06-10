#pragma once

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include "PixelWindow.h"
#include "PixelGraphicsPipeline.h"
#include "PixelComputePipeline.h"
#include "Utility.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cstring>

const int MAX_FRAME_DRAWS = 2; //we always have "MAX_FRAME_DRAWS" being drawing at once.
static float dofFocus = 13.152946438f;
static bool autoFocus = false;
static bool autoFocusFinished = true;
static float deltaFocus = 0.0f;
static glm::uvec2 mouseCoord = {0,0};
static glm::uvec2 lastClicked = {28,156};
static ImColor color = ImColor(0.0,0.0f,0.0f,1.0f);
static int MAX_COMPUTE_SAMPLE = 1;
static bool guiItemHovered = false;

class PixelRenderer
{
public:
    PixelRenderer() = default;
	PixelRenderer(const PixelRenderer&) = delete;

    PixelRenderer& operator=(const PixelRenderer&) = delete;
	~PixelRenderer() = default;

	int initRenderer();
    void addScene(PixelScene* pixScene);
    void draw();
    void run();
	bool windowShouldClose();
	void cleanup();

    float currentTime = 0;

private:



	//vulkan component
#ifdef __APPLE__
	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    "VK_KHR_portability_subset",
    "VK_EXT_descriptor_indexing"
	};
#else
    const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
#endif

    //logical and physical device
    PixBackend mainDevice;

    //window component
    PixelWindow pixWindow{};

    //physical device features the logical device will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};

	VkInstance instance{};
	VkQueue graphicsQueue{};
	VkQueue presentationQueue{};
	VkQueue computeQueue{};
	VkSurfaceKHR surface{};
	VkSwapchainKHR swapChain{};
    std::vector<VkFramebuffer> swapchainFramebuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkCommandBuffer> computeCommandBuffers;
    std::vector<std::unique_ptr<PixelGraphicsPipeline>> graphicsPipelines;
    PixelComputePipeline computePipeline;

    //images
    std::vector<PixelImage> swapChainImages;
    PixelImage depthImage;
    PixelImage emptyTexture;
    VkSampler imageSampler{};

	// Utility
	VkFormat swapChainImageFormat{};
	VkExtent2D swapChainExtent{};

    // Pools
    VkCommandPool graphicsCommandPool{};
    VkCommandPool computeCommandPool{};

    // gui ressources
    VkDescriptorPool imguiPool{};
    ImGuiIO* io{};
    ImDrawData* draw_data{};

    //validation layer component
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
	VkDebugUtilsMessengerEXT debugMessenger{};

    //synchronization component
    std::vector<VkSemaphore> imageAvailableSemaphore;
    std::vector<VkSemaphore> renderFinishedSemaphore;
    std::vector<VkSemaphore> computeFinishedSemaphore;
    std::vector<VkFence> inFlightDrawFences;
    std::vector<VkFence> inFlightComputeFences;
    int currentFrame = 0;
    std::array<glm::vec3, 512> randomArray;

    //objects
    std::vector<PixelScene> scenes;

	//---------vulkan functions
	//create functions
	void createInstance();
	void setupPhysicalDevice();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
    void createGraphicsPipelines();
    void createFramebuffers();
    void createCommandPools();
    void createCommandBuffers();
    void createComputeCommandBuffers();
	void createScene();
    void createDepthBuffer();
	void initializeScenes();
    void createSynchronizationObjects();
    void recordCommands(uint32_t currentImageIndex);
    void recordComputeCommands(uint32_t currentImageIndex);
    VkCommandBuffer beginSingleUseCommandBuffer();
    void submitAndEndSingleUseCommandBuffer(VkCommandBuffer* commandBuffer);
	QueueFamilyIndices setupQueueFamilies(VkPhysicalDevice device);
	void init_io();
    void init_compute();
	void preDraw();

    //gui functions
    bool ColorPicker(const char* label, ImColor* color);
    void init_imgui();
    void imGuiParameters();

	//descriptor Set (for scene initialization)
	void createDescriptorPool(PixelScene* pixScene);
	void createDescriptorSets(PixelScene* pixScene);
	void createUniformBuffers(PixelScene* pixScene);
    void updateComputeTextureDescriptor();

    //debug validation layer
    void setupDebugMessenger();
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	//helper functions
	bool checkIfPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	VkExtent2D chooseSwapChainExtent(VkSurfaceCapabilitiesKHR surfaceCapabilities);
    void transitionImageLayout(VkImage imageToTransition, VkImageLayout currentLayout, VkImageLayout newLayout);
    void transitionImageLayoutUsingCommandBuffer(VkCommandBuffer commandBuffer, VkImage imageToTransition, VkImageLayout currentLayout, VkImageLayout newLayout);
    void createBuffer(VkDeviceSize bufferSize,
                     VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags bufferproperties,
                     VkBuffer* buffer, VkDeviceMemory* bufferMemory);
    void copySrcBuffertoDstBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize bufferSize);
    void copySrcBuffertoDstImage(VkBuffer srcBuffer, VkImage dstImageBuffer, uint32_t width, uint32_t height);

    void initializeObjectBuffers(PixelObject* pixObject);
    void createVertexBuffer(PixelObject* pixObject);
    void createIndexBuffer(PixelObject* pixObject);
    void createTextureBuffer(PixelImage* pixImage);
    void createTextureSampler();

	//getter functions
	SwapchainDetails getSwapChainDetails(VkPhysicalDevice device);

};

