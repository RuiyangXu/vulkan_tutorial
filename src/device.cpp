#include "device.hpp"
#include "instance.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>


bool isDeviceSuitable(VkPhysicalDevice device) {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
}

Device::Device(const Instance& instance) : instance_(instance), queueFamilyProperties_{}, physicalDevice_{}, device_{}
{
	init();
	createDevice();
	setGraphicsQueue();
}

uint32_t Device::pickGraphicsQueueFamily()
{
	for (size_t index = 0; index < queueFamilyProperties_.size(); ++index) {
		if (queueFamilyProperties_[index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			return index;
		}
	}
	throw std::runtime_error("cannot pick a graphics family queue");
}

Device::~Device()
{
	if (device_) {
		vkDestroyDevice(device_, nullptr);
	}
}

void Device::init()
{
	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance_.vkInstance_, &physicalDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance_.vkInstance_, &physicalDeviceCount, physicalDevices.data());
	for (const auto& physicalDevice : physicalDevices) {
		if (isDeviceSuitable(physicalDevice)) {
			physicalDevice_ = physicalDevice;
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
			queueFamilyProperties_.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties_.data());
			return;
		}
	}
}

void Device::createDevice()
{
	float queuePriority = 1.0f;
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.queueFamilyIndex = this->pickGraphicsQueueFamily();
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};


	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	if(vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &device_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
	}
}

void Device::setGraphicsQueue()
{
	vkGetDeviceQueue(device_, pickGraphicsQueueFamily(), 0, &graphicsQueue_);
}
