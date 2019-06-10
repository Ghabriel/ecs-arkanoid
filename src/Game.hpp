#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "engine-glue/ecs.hpp"
#include "engine/state-management/StateMachine.hpp"
// #include "states/RunningState.hpp"
#include "states/WaitingState.hpp"

class Game {
 public:
    void init(float width, float height) {
        stateMachine.registerState("waiting", std::make_unique<WaitingState>(world, stateMachine));
        // stateMachine.registerState("running", std::make_unique<RunningState>(world, stateMachine));
        stateMachine.pushState("waiting");
    }

    void update(const sf::Time& elapsedTime) {
        stateMachine.getState().update(elapsedTime);
    }

    void render(sf::RenderWindow& window) {
        stateMachine.getState().render(window);
    }

 private:
    ecs::ComponentManager world;
    state::StateMachine stateMachine;
};
