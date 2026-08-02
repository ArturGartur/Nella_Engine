#include "Engine.h"
#include <iostream>

int main() {
    Engine engine;
    try {
        engine.run();
    }catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
    return 0;
}