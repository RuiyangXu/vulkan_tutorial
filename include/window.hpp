#pragma once

#include <GLFW/glfw3.h>

class VulkanWindow {
public:
    VulkanWindow();
    ~VulkanWindow();
    VulkanWindow(const VulkanWindow&) = delete;
    VulkanWindow& operator=(const VulkanWindow&) = delete;

    void run();

    // Getter for GLFW window
    GLFWwindow* getWindow() const { return window_; }

private:
    GLFWwindow* window_;
};