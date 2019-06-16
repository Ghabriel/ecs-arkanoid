#pragma once

#include "../engine-glue/ecs.hpp"
#include "../engine/state-management/include.hpp"
#include "../systems/collision-handler-system/include.hpp"
#include "../systems/input-system/include.hpp"
#include "../systems/launching-system/include.hpp"
#include "../systems/movement-system/include.hpp"
#include "../systems/rendering-system/include.hpp"

class RunningState : public state::State {
 public:
    RunningState(
        ecs::World& world,
        state::StateMachine& stateMachine
    ) : world(world), stateMachine(stateMachine) {
        collisionListenerId = world.createEntity();
    }

    virtual void onEnter() override {
        useLaunchingSystem(world);

        useEffect([this] {
            auto callback = [this](ecs::Entity ballId, ecs::Entity objectId) {
                useCollisionHandlerSystem(world, ballId, objectId);
            };

            world.addComponent(collisionListenerId, BallCollisionListener { callback });

            return [this] {
                world.removeComponent<BallCollisionListener>(collisionListenerId);
            };
        });
    }

    virtual void update(const sf::Time& elapsedTime) override {
        unsigned elapsedTimeMicro = elapsedTime.asMicroseconds();
        float normalizedElapsedTime = elapsedTimeMicro / 1000000.0;

        useInputSystem(world);
        useMovementSystem(world, normalizedElapsedTime);
    }

    virtual void render(sf::RenderWindow& window) override {
        useRenderingSystem(world, window);
    }

 private:
    ecs::World& world;
    state::StateMachine& stateMachine;
    ecs::Entity collisionListenerId;
};
