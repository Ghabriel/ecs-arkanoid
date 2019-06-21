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
auto partialApply(F fn, Args&&... fixedArgs) {
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
        listenerId = world.createEntity();
    }

    virtual void onEnter() override {
        useLaunchingSystem(world);

        listenToBallPaddleCollisions();
        listenToBounceCollisions();
        listenToPaddlePowerUpCollisions();
        listenToPaddleWallCollisions();
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
    ecs::Entity listenerId;

    template<typename T>
    void useToggleComponentEffect(ecs::Entity entity, const T& component) {
        EffectState::useToggleComponentEffect(world, entity, component);
    }

    void listenToBallPaddleCollisions() {
        auto callback = partialApply(useBallPaddleCollisionSystem, world);
        useToggleComponentEffect(listenerId, BallPaddleCollisionListener { callback });
    }

    void listenToBounceCollisions() {
        auto callback = partialApply(useBounceCollisionSystem, world);
        useToggleComponentEffect(listenerId, BallObjectsCollisionListener { callback });
    }

    void listenToPaddlePowerUpCollisions() {
        auto callback = partialApply(usePaddlePowerUpCollisionSystem, world);
        useToggleComponentEffect(listenerId, PaddlePowerUpCollisionListener { callback });
    }

    void listenToPaddleWallCollisions() {
        auto callback = partialApply(usePaddleWallCollisionSystem, world);
        useToggleComponentEffect(listenerId, PaddleWallCollisionListener { callback });
    }

    void listenToGameOver() {
        auto callback = [this] {
            world.clear();
            stateMachine.pushState("waiting");
        };

        useToggleComponentEffect(listenerId, GameOverListener { callback });
    }
};
