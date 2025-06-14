#include <GLFW/glfw3.h>

class VulkanWindow {
    public:
        VulkanWindow();
        ~VulkanWindow();
        VulkanWindow(const VulkanWindow&) = delete;
        VulkanWindow operator = (const VulkanWindow&) = delete;
        void run();
    private:
        GLFWwindow* window_;
};