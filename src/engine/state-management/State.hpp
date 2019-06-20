#pragma once

#include <functional>
#include <queue>
#include <SFML/Graphics.hpp>

namespace state {
    class State {
     public:
        virtual ~State() = default;

        virtual void onEnter() { }
        virtual void update(const sf::Time& elapsedTime) = 0;
        virtual void render(sf::RenderWindow& window) = 0;

        void onExit() {
            while (!cleanupFunctions.empty()) {
                const auto& fn = cleanupFunctions.front();
                fn();
                cleanupFunctions.pop();
            }
        }

        template<typename F>
        void useEffect(F fn) {
            auto cleanupFn = fn();
            cleanupFunctions.push(cleanupFn);
        }

     private:
        std::queue<std::function<void()>> cleanupFunctions;
    };
}
