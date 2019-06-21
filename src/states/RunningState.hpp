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

        listenToCollisions();
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

    void listenToCollisions() {
        listenToBallPaddleCollisions();
        listenToBallBrickCollisions();
        listenToBallWallCollisions();
        listenToPaddlePowerUpCollisions();
        listenToPaddleWallCollisions();
    }

    void listenToBallPaddleCollisions() {
        auto callback = partialApply(useCollisionSystem<Ball, Paddle>, world);
        useToggleComponentEffect(listenerId, CollisionListener<Ball, Paddle> { callback });
    }

    void listenToBallBrickCollisions() {
        auto callback = partialApply(useCollisionSystem<Ball, Brick>, world);
        useToggleComponentEffect(listenerId, CollisionListener<Ball, Brick> { callback });
    }

    void listenToBallWallCollisions() {
        auto callback = partialApply(useCollisionSystem<Ball, Wall>, world);
        useToggleComponentEffect(listenerId, CollisionListener<Ball, Wall> { callback });
    }

    void listenToPaddlePowerUpCollisions() {
        auto callback = partialApply(useCollisionSystem<Paddle, PowerUp>, world);
        useToggleComponentEffect(listenerId, CollisionListener<Paddle, PowerUp> { callback });
    }

    void listenToPaddleWallCollisions() {
        auto callback = partialApply(useCollisionSystem<Paddle, Wall>, world);
        useToggleComponentEffect(listenerId, CollisionListener<Paddle, Wall> { callback });
    }

    void listenToGameOver() {
        auto callback = [this] {
            world.clear();
            stateMachine.pushState("waiting");
        };

        useToggleComponentEffect(listenerId, GameOverListener { callback });
    }
};
