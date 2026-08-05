#pragma once
#include <entt/entt.hpp>
#include <string>
#include "Components.h"

namespace Nella {

    class World {
    public:
        World() = default;
        ~World() = default;


        entt::entity CreateEntity(const std::string& name = "Empty_Entity");

        void Update(float deltaTime);

        entt::registry& GetRegistry() { return m_Registry; }

    private:
        entt::registry m_Registry;
    };

}