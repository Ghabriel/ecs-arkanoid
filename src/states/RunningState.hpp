#pragma once

#include "../engine-glue/ecs.hpp"
#include "../engine/state-management/include.hpp"
#include "../systems/collision-handler-system/include.hpp"
#include "../systems/collision-system/include.hpp"
#include "../systems/game-over-system/include.hpp"
#include "../systems/input-system/include.hpp"
#include "../systems/launching-system/include.hpp"
#include "../systems/movement-system/include.hpp"
#include "../systems/rendering-system/include.hpp"

template<typename F, typename... Args>
auto make_forwarder(F fn, Args&&... fixedArgs) {
    return [&, fn](auto&&... extraArgs) {
        fn(
            std::forward<Args>(fixedArgs)...,
            std::forward<decltype(extraArgs)>(extraArgs)...
        );
    };
}

class RunningState : public state::EffectState {
 public:
    RunningState(
        ecs::World& world,
        state::StateMachine& stateMachine
    ) : world(world), stateMachine(stateMachine) {
        collisionListenerId = world.createEntity();
    }

    virtual void onEnter() override {
        useLaunchingSystem(world);

        listenToPaddleCollisions();
        listenToBounceCollisions();
        listenToGameOver();
    }

    virtual void update(const sf::Time& elapsedTime) override {
        unsigned elapsedTimeMicro = elapsedTime.asMicroseconds();
        float normalizedElapsedTime = elapsedTimeMicro / 1000000.0;

        useInputSystem(world);
        useCollisionSystem(world, normalizedElapsedTime);
        useMovementSystem(world, normalizedElapsedTime);
        useGameOverSystem(world);
    }

    virtual void render(sf::RenderWindow& window) override {
        useRenderingSystem(world, window);
    }

 private:
    ecs::World& world;
    state::StateMachine& stateMachine;
    ecs::Entity collisionListenerId;

    void listenToPaddleCollisions() {
        auto callback = make_forwarder(usePaddleCollisionHandlerSystem, world);

        useToggleComponentEffect(
            world,
            collisionListenerId,
            BallPaddleCollisionListener { callback }
        );
    }

    void listenToBounceCollisions() {
        auto callback = make_forwarder(useBounceCollisionHandlerSystem, world);

        useToggleComponentEffect(
            world,
            collisionListenerId,
            BallObjectsCollisionListener { callback }
        );
    }

    void listenToGameOver() {
        auto callback = [this] {
            world.clear();
            stateMachine.pushState("waiting");
        };

        useToggleComponentEffect(
            world,
            collisionListenerId,
            GameOverListener { callback }
        );
    }
};
