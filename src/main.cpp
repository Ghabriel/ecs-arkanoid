#include <SFML/Graphics.hpp>
#include "constants.hpp"
#include "Game.hpp"

int main(int, char**) {
    using constants::WINDOW_WIDTH;
    using constants::WINDOW_HEIGHT;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ECS Pong");
    Game game;
    game.init(WINDOW_WIDTH, WINDOW_HEIGHT);

    window.setFramerateLimit(60);
    window.setPosition({200, 100});

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        sf::Time elapsedTime = clock.restart();
        game.update(elapsedTime);
        game.render(window);
        window.display();
    }
}
