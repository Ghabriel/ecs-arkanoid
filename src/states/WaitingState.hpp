#pragma once

#include "../engine-glue/ecs.hpp"
#include "../engine/state-management/include.hpp"
#include "../systems/level-loading-system/level-loading-system.hpp"
#include "../systems/rendering-system/rendering-system.hpp"

class WaitingState : public state::State {
 public:
    WaitingState(
        ecs::ComponentManager& world,
        state::StateMachine& stateMachine
    ) : world(world), stateMachine(stateMachine) { }

    virtual void onEnter() override {
        useLevelLoadingSystem(world);
    }

    virtual void update(const sf::Time& elapsedTime) override {
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        //     stateMachine.pushState("running");
        // }
    }

    virtual void render(sf::RenderWindow& window) override {
        useRenderingSystem(world, window);
    }

 private:
    ecs::ComponentManager& world;
    state::StateMachine& stateMachine;
};
