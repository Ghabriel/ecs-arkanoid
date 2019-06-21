#pragma once

#include <functional>
#include <queue>
#include "../ecs/World.hpp"
#include "State.hpp"

namespace state {
    class EffectState : public State {
     public:
        void onExit() override {
            while (!cleanupFunctions.empty()) {
                const auto& fn = cleanupFunctions.front();
                fn();
                cleanupFunctions.pop();
            }
        }

     protected:
        template<typename F>
        void useEffect(F fn) {
            auto cleanupFn = fn();
            cleanupFunctions.push(cleanupFn);
        }

        template<typename T, typename ECS>
        void useToggleComponentEffect(
            ecs::GenericWorld<ECS>& world,
            ecs::Entity entity,
            const T& component
        ) {
            useEffect([&world, entity, &component] {
                world.addComponent(entity, component);

                return [&world, entity] {
                    world.template removeComponent<T>(entity);
                };
            });
        }

     private:
        std::queue<std::function<void()>> cleanupFunctions;
    };
}
