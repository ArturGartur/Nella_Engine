#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <optional>
#include <limits>
#include <string>

class Engine {

    public:
    void run();

private:

    static std::vector<char> readFile(const std::string& filename);

    void createGraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char>& code);

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
    void pickPhysicalDevice();
    void createSurface();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice deviceToCheck);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice deviceToCheck);

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createLogicalDevice();
    void createSwapChain();

    GLFWwindow* window = nullptr;
    VkInstance instance;
    VkSurfaceKHR surface;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkImageView> swapChainImageViews;
    void createImageViews();

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    void createSyncObjects();

    void createCommandPool();
    void createCommandBuffer();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();

    VkRenderPass renderPass;
    void createRenderPass();

    std::vector<VkFramebuffer> swapChainFramebuffers;
    void createFramebuffers();

    const uint32_t WIDTH = 1280;
    const uint32_t HEIGHT = 720;
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    VkPhysicalDevice physDevice = VK_NULL_HANDLE;

    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

};
