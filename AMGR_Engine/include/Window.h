#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>

class Window {
public:
    Window(int w, int h, std::string name);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window); }
    void pollEvents() { glfwPollEvents(); }

    GLFWwindow* getGLFWwindow() const { return window; }
    void setWindowTitle(const std::string& title) { glfwSetWindowTitle(window, title.c_str()); }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
private:
    void initWindow();

    int width;
    int height;
    std::string windowName;
    GLFWwindow* window;
};
