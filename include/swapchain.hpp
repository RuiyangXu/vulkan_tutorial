#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "device.hpp"
#include "instance.hpp"
#include "window.hpp"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain {
public:
    SwapChain(const Device& device, const Instance& instance, const VulkanWindow& window);
    ~SwapChain();
    
    // Delete copy constructor and assignment operator
    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    
    // Getters
    VkSwapchainKHR getSwapChain() const { return swapChain_; }
    VkSurfaceKHR getSurface() const { return surface_; }
    VkFormat getSwapChainImageFormat() const { return swapChainImageFormat_; }
    VkExtent2D getSwapChainExtent() const { return swapChainExtent_; }
    const std::vector<VkImage>& getSwapChainImages() const { return swapChainImages_; }
    const std::vector<VkImageView>& getSwapChainImageViews() const { return swapChainImageViews_; }
    uint32_t getImageCount() const { return static_cast<uint32_t>(swapChainImages_.size()); }
    
    // Swap chain operations
    VkResult acquireNextImage(uint32_t* imageIndex, VkSemaphore semaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);
    VkResult presentImage(uint32_t imageIndex, VkSemaphore* waitSemaphores = nullptr, uint32_t waitSemaphoreCount = 0);
    
    // Recreation for window resize
    void recreateSwapChain();
    
private:
    // References to other components
    const Device& device_;
    const Instance& instance_;
    const VulkanWindow& window_;
    
    // Vulkan objects
    VkSurfaceKHR surface_;
    VkSwapchainKHR swapChain_;
    std::vector<VkImage> swapChainImages_;
    std::vector<VkImageView> swapChainImageViews_;
    VkFormat swapChainImageFormat_;
    VkExtent2D swapChainExtent_;
    
    // Present queue (might be different from graphics queue)
    VkQueue presentQueue_;
    uint32_t presentQueueFamily_;
    
    // Helper methods
    void createSurface();
    void createSwapChain();
    void createImageViews();
    void cleanupSwapChain();
    
    // Support query methods
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    
    // Queue family methods
    uint32_t findPresentQueueFamily() const;
    bool isDeviceSuitableForPresentation(VkPhysicalDevice device);
};
