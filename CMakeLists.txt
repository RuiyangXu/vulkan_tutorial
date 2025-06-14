cmake_minimum_required(VERSION 4.0.2)
project(vulkan_tutorial)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED FATAL_ERROR)

file(GLOB helloworld_SRC
     "src/*.cpp"
)
add_executable(vulkan_tutorial ${helloworld_SRC})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(vulkan_tutorial glfw)
