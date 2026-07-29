// Определяем макрос ДО подключения glfw, чтобы он автоматически подтянул заголовки Vulkan
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Инициализируем оконную библиотеку
    if (!glfwInit()) {
        std::cerr << "Ошибка: Не удалось инициализировать GLFW!" << std::endl;
        return -1;
    }

    // Говорим GLFW, что мы будем использовать Vulkan, а не OpenGL по умолчанию
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Запрещаем изменение размера окна для простоты на старте
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Создаем окно (ширина, высота, название, монитор, общие ресурсы)
    GLFWwindow* window = glfwCreateWindow(1280, 720, "AMGR Engine - Test Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ошибка: Не удалось создать окно!" << std::endl;
        glfwTerminate();
        return -1;
    }

    std::cout << "Окно успешно создано! Движок запущен." << std::endl;

    // Главный цикл движка (крутится, пока не нажмут крестик)
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Обработка событий (нажатия клавиш, мыши)
    }

    // Уборка за собой перед выходом
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}