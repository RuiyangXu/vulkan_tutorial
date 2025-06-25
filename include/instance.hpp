#pragma once

#include <vulkan/vulkan.h>

class Instance {
public:
    Instance();
    ~Instance();
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    VkInstance vkInstance_;
};