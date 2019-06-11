#pragma once

#include <SFML/Graphics.hpp>
#include "../engine-glue/ecs.hpp"

void useRenderingSystem(ecs::ComponentManager& world, sf::RenderWindow& window) {
    world.findAll<Visible>()
        .join<Circle>()
        .join<Position>()
        .forEach(
            [&window](const Circle& circle, const Position& pos) {
                sf::CircleShape c(circle.radius);
                c.setFillColor(sf::Color::White);
                c.setPosition({
                    pos.x - circle.radius,
                    pos.y - circle.radius
                });

                window.draw(c);
            }
        );

    world.findAll<Visible>()
        .join<Rectangle>()
        .join<Position>()
        .forEach(
            [&window](const Rectangle& rect, const Position& pos) {
                sf::RectangleShape r({ rect.width, rect.height });
                r.setFillColor(sf::Color::White);
                r.setPosition({
                    pos.x - rect.width / 2,
                    pos.y - rect.height / 2
                });

                window.draw(r);
            }
        );
}
