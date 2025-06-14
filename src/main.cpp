#include <iostream>
#include "../include/window.hpp"

int main(int, char**){
    std::cout << "Hello, from vulkan_tutorial!\n";
    VulkanWindow window;
    window.run();
}
