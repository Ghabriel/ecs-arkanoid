#pragma once

#include "State.hpp"

namespace state {
    class NullState : public State {
     public:
        virtual void update(const sf::Time& elapsedTime) override { }
        virtual void render(sf::RenderWindow& window) override { }
    };
}
