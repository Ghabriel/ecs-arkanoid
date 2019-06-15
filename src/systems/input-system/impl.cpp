#include "include.hpp"

#include <SFML/Graphics.hpp>
#include "../../constants.hpp"

void useInputSystem(ecs::ComponentManager& world) {
    world.findAll<Input>()
        .forEach(
            [&world](ecs::Entity id) {
                using constants::PADDLE_VELOCITY;

                Velocity velocity;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    velocity = { -PADDLE_VELOCITY, 0 };
                } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    velocity = { PADDLE_VELOCITY, 0 };
                } else {
                    world.removeComponent<Velocity>(id);
                    return;
                }

                world.replaceComponent(id, velocity);
            }
        );
}
