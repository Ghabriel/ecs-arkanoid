#pragma once

#include <SFML/Graphics.hpp>

namespace state {
    class State {
     public:
        virtual ~State() = default;

        virtual void onEnter() { }
        virtual void onExit() { }
        virtual void update(const sf::Time& elapsedTime) = 0;
        virtual void render(sf::RenderWindow& window) = 0;
    };
}
