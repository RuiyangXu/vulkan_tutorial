cmake_minimum_required(VERSION 3.28.0)
project(vulkan_tutorial)

set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED FATAL_ERROR)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.4.bin.WIN64/include)
file(GLOB sourcefiles
     "src/*.cpp"
)
add_executable(${PROJECT_NAME} ${sourcefiles})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/libs/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.lib Vulkan::Vulkan)
