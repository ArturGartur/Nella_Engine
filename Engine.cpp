#include "Engine.h"
#include <iostream>

void Engine::run() {

    initWindow();
    initVulkan();
    mainLoop();
    cleanup();

}

void Engine::initWindow() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "AMGR Engine", nullptr, nullptr);
}

void Engine::initVulkan() {

}

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void Engine::cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}