#include "window.hpp"

VulkanWindow::VulkanWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_ = glfwCreateWindow(800, 600, "vulkan_tutorial", nullptr, nullptr);
}

VulkanWindow::~VulkanWindow() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void VulkanWindow::run() {
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();
    }
}
