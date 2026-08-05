#pragma once
#include <string>
#include <cstdint>

namespace Nella {
    
    struct TagComponent {
        std::string Name;
    };

    struct TransformComponent {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;
    };

    struct UnitStatsComponent {
        int Health = 100;
        int MaxHealth = 100;
        float MovementSpeed = 5.0f;
        int ArmorThickness = 10;
    };

}