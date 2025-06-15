#include <iostream>
#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"

int main(int, char**){
    VulkanWindow window;
    Instance instance;
    Device device(instance);
    window.run();
}
