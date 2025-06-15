#include "instance.hpp"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdexcept>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

std::vector<const char*> getRequiredExtensions() {
	uint32_t glfwExtensionCount;
	 const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT> (vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


Instance::Instance(){
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.apiVersion = VK_API_VERSION_1_3;
	applicationInfo.pApplicationName = "vulkan_tutorial";
	applicationInfo.pEngineName = "engine";

	std::vector<const char*> extentions = getRequiredExtensions();

	VkInstanceCreateInfo instanceCreateInfo{};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extentions.size());
	instanceCreateInfo.ppEnabledExtensionNames = extentions.data();
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	if (enableValidationLayers) {
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		instanceCreateInfo.enabledLayerCount = 0;
	}
	if (enableValidationLayers) {
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		debugCreateInfo.pUserData = nullptr; // Optional
		instanceCreateInfo.pNext = &debugCreateInfo;
	}


	if (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance_) != VK_SUCCESS) {
		throw std::runtime_error("vulkan instance not initialized properly");
	}
}

Instance::~Instance() {
	if (vkInstance_) {
		vkDestroyInstance(vkInstance_, nullptr);
	}
}