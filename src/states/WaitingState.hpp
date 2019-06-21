#pragma once

#include "../engine-glue/ecs.hpp"
#include "../engine/state-management/include.hpp"
#include "../systems/collision-system/include.hpp"
#include "../systems/input-system/include.hpp"
#include "../systems/level-loading-system/include.hpp"
#include "../systems/movement-system/include.hpp"
#include "../systems/rendering-system/include.hpp"

class WaitingState : public state::EffectState {
 public:
    WaitingState(
        ecs::World& world,
        state::StateMachine& stateMachine
    ) : world(world), stateMachine(stateMachine) { }

    virtual void onEnter() override {
        useLevelLoadingSystem(world);

        useEffect([this] {
            ecs::Entity ball = world.unique<Ball>();
            ecs::Entity paddle = world.unique<Paddle>();

            const Position& ballPos = world.getData<Position>(ball);
            const Position& paddlePos = world.getData<Position>(paddle);

            world.addComponent(ball, Link { paddle, ballPos - paddlePos });

            return [this, ball] {
                world.removeComponent<Link>(ball);
            };
        });
    }

    virtual void update(const sf::Time& elapsedTime) override {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            stateMachine.pushState("running");
            return;
        }

        unsigned elapsedTimeMicro = elapsedTime.asMicroseconds();
        float normalizedElapsedTime = elapsedTimeMicro / 1000000.0;

        useInputSystem(world);
        useCollisionSystem(world, normalizedElapsedTime);
        useMovementSystem(world, normalizedElapsedTime);
    }

    virtual void render(sf::RenderWindow& window) override {
        useRenderingSystem(world, window);
    }

 private:
    ecs::World& world;
    state::StateMachine& stateMachine;
};
