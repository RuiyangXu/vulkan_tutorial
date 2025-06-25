#include "swapchain.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <algorithm>
#include <limits>

SwapChain::SwapChain(const Device& device, const Instance& instance, const VulkanWindow& window)
    : device_(device), instance_(instance), window_(window),
      surface_(VK_NULL_HANDLE), swapChain_(VK_NULL_HANDLE),
      swapChainImageFormat_(VK_FORMAT_UNDEFINED), swapChainExtent_{0, 0},
      presentQueue_(VK_NULL_HANDLE), presentQueueFamily_(0) {
    createSurface();
    presentQueueFamily_ = findPresentQueueFamily();
    vkGetDeviceQueue(device_.getDevice(), presentQueueFamily_, 0, &presentQueue_);
    createSwapChain();
    createImageViews();
}

SwapChain::~SwapChain() {
    cleanupSwapChain();
    
    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance_.vkInstance_, surface_, nullptr);
    }
}

void SwapChain::createSurface() {
    if (glfwCreateWindowSurface(instance_.vkInstance_, window_.getWindow(), nullptr, &surface_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    // Get surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

    // Get surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
    }

    // Get present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount,
                                                details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    // Prefer SRGB if available
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    // Otherwise, just return the first format
    return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    // Prefer mailbox mode (triple buffering) if available
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // FIFO is guaranteed to be available
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window_.getWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width,
                                      capabilities.minImageExtent.width,
                                      capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                       capabilities.minImageExtent.height,
                                       capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

uint32_t SwapChain::findPresentQueueFamily() const {
    // For simplicity, we'll assume the graphics queue family also supports presentation
    // In a more robust implementation, you might want to find a separate present queue
    uint32_t graphicsFamily = device_.getGraphicsQueueFamily();

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device_.getPhysicalDevice(), graphicsFamily,
                                       surface_, &presentSupport);

    if (presentSupport) {
        return graphicsFamily;
    }

    throw std::runtime_error("failed to find a suitable present queue family!");
}

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device_.getPhysicalDevice());
    
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && 
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface_;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    uint32_t queueFamilyIndices[] = {device_.getGraphicsQueueFamily(), presentQueueFamily_};
    
    if (device_.getGraphicsQueueFamily() != presentQueueFamily_) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }
    
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    if (vkCreateSwapchainKHR(device_.getDevice(), &createInfo, nullptr, &swapChain_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    
    // Get swap chain images
    vkGetSwapchainImagesKHR(device_.getDevice(), swapChain_, &imageCount, nullptr);
    swapChainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(device_.getDevice(), swapChain_, &imageCount, swapChainImages_.data());
    
    swapChainImageFormat_ = surfaceFormat.format;
    swapChainExtent_ = extent;
}

void SwapChain::createImageViews() {
    swapChainImageViews_.resize(swapChainImages_.size());

    for (size_t i = 0; i < swapChainImages_.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages_[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat_;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device_.getDevice(), &createInfo, nullptr,
                            &swapChainImageViews_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

void SwapChain::cleanupSwapChain() {
    for (auto imageView : swapChainImageViews_) {
        vkDestroyImageView(device_.getDevice(), imageView, nullptr);
    }
    swapChainImageViews_.clear();

    if (swapChain_ != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device_.getDevice(), swapChain_, nullptr);
        swapChain_ = VK_NULL_HANDLE;
    }
}

VkResult SwapChain::acquireNextImage(uint32_t* imageIndex, VkSemaphore semaphore, VkFence fence) {
    return vkAcquireNextImageKHR(device_.getDevice(), swapChain_, UINT64_MAX,
                               semaphore, fence, imageIndex);
}

VkResult SwapChain::presentImage(uint32_t imageIndex, VkSemaphore* waitSemaphores,
                                uint32_t waitSemaphoreCount) {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = waitSemaphoreCount;
    presentInfo.pWaitSemaphores = waitSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    return vkQueuePresentKHR(presentQueue_, &presentInfo);
}

void SwapChain::recreateSwapChain() {
    // Wait for device to be idle
    vkDeviceWaitIdle(device_.getDevice());

    // Clean up old swap chain
    cleanupSwapChain();

    // Create new swap chain
    createSwapChain();
    createImageViews();
}
