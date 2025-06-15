#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include "instance.hpp"
#include <vector>

class Device {
public:
	Device(const Instance& instance);
	~Device();
private:
	std::vector<VkQueueFamilyProperties> queueFamilyProperties_;
	const Instance& instance_;
	VkPhysicalDevice physicalDevice_;
	VkDevice device_;
	VkQueue graphicsQueue_;
	void init();
	void createDevice();
	void setGraphicsQueue();
	uint32_t pickGraphicsQueueFamily();
};