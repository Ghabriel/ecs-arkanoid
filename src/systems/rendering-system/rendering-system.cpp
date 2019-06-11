#include "rendering-system.hpp"

#include "../../constants.hpp"

void renderCircles(ecs::ComponentManager&, sf::RenderWindow&);
void renderRectangles(ecs::ComponentManager&, sf::RenderWindow&);

void useRenderingSystem(ecs::ComponentManager& world, sf::RenderWindow& window) {
    renderCircles(world, window);
    renderRectangles(world, window);
}

void renderCircles(ecs::ComponentManager& world, sf::RenderWindow& window) {
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
}

void renderRectangles(ecs::ComponentManager& world, sf::RenderWindow& window) {
    world.findAll<Visible>()
        .join<Rectangle>()
        .join<Position>()
        .forEach(
            [&window](const Rectangle& rect, const Position& pos) {
                using constants::BRICK_BORDER_WIDTH;

                sf::RectangleShape r({
                    rect.width - 2 * BRICK_BORDER_WIDTH,
                    rect.height - 2 * BRICK_BORDER_WIDTH
                });
                r.setFillColor(sf::Color::White);
                r.setOutlineColor(sf::Color::Blue);
                r.setOutlineThickness(BRICK_BORDER_WIDTH);
                r.setPosition({
                    pos.x + BRICK_BORDER_WIDTH - rect.width / 2,
                    pos.y + BRICK_BORDER_WIDTH - rect.height / 2
                });

                window.draw(r);
            }
        );
}
