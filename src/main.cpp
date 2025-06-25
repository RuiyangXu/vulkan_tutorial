#include <iostream>
#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "swapchain.hpp"

int main(int, char**) {
    try {
        VulkanWindow window;
        Instance instance;
        Device device(instance);
        SwapChain swapChain(device, instance, window);

        std::cout << "Swap chain created successfully!" << std::endl;
        std::cout << "Image count: " << swapChain.getImageCount() << std::endl;
        std::cout << "Image format: " << swapChain.getSwapChainImageFormat() << std::endl;
        std::cout << "Extent: " << swapChain.getSwapChainExtent().width
                  << "x" << swapChain.getSwapChainExtent().height << std::endl;

        window.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
