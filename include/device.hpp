#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "instance.hpp"

class Device {
public:
    Device(const Instance& instance);
    ~Device();

    // Getters for Vulkan handles
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    VkDevice getDevice() const { return device_; }
    VkQueue getGraphicsQueue() const { return graphicsQueue_; }
    uint32_t getGraphicsQueueFamily() const { return pickGraphicsQueueFamily(); }

private:
    std::vector<VkQueueFamilyProperties> queueFamilyProperties_;
    const Instance& instance_;
    VkPhysicalDevice physicalDevice_;
    VkDevice device_;
    VkQueue graphicsQueue_;

    void init();
    void createDevice();
    void setGraphicsQueue();
    uint32_t pickGraphicsQueueFamily() const;
};