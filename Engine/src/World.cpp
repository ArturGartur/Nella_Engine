#include "World.h"
#include <iostream>

namespace Nella {

    entt::entity World::CreateEntity(const std::string& name) {
        entt::entity entity = m_Registry.create();
        m_Registry.emplace<TagComponent>(entity, name);
        m_Registry.emplace<TransformComponent>(entity);
        return entity;
    }

    void World::Update(float deltaTime) {

        auto view = m_Registry.view<TagComponent, UnitStatsComponent>();

        for (auto entity : view) {
            auto& tag = view.get<TagComponent>(entity);
            auto& stats = view.get<UnitStatsComponent>(entity);

            if (stats.Health < stats.MaxHealth) {

                stats.Health += 1;

                std::cout << "[System] " << tag.Name << " regenerating. HP: "
                          << stats.Health << "/" << stats.MaxHealth << std::endl;
            }
        }
    }
}