#include "include.hpp"

#include "../../constants.hpp"

static void renderCircles(ecs::World&, sf::RenderWindow&);
static void renderRectangles(ecs::World&, sf::RenderWindow&);

void useRenderingSystem(ecs::World& world, sf::RenderWindow& window) {
    renderCircles(world, window);
    renderRectangles(world, window);
}

void renderCircles(ecs::World& world, sf::RenderWindow& window) {
    world.findAll<Visible>()
        .join<Circle>()
        .join<Position>()
        .join<Style>()
        .forEach(
            [&window](const Circle& circle, const Position& pos, const Style& style) {
                const sf::Color fillColor = style.fillColor;
                const sf::Color borderColor = style.borderColor;
                const float borderThickness = style.borderThickness;

                sf::CircleShape c(circle.radius - borderThickness);
                c.setFillColor(fillColor);
                c.setOutlineColor(borderColor);
                c.setOutlineThickness(borderThickness);
                c.setPosition({
                    pos.x + borderThickness - circle.radius,
                    pos.y + borderThickness - circle.radius
                });

                window.draw(c);
            }
        );
}

void renderRectangles(ecs::World& world, sf::RenderWindow& window) {
    world.findAll<Visible>()
        .join<Rectangle>()
        .join<Position>()
        .join<Style>()
        .forEach(
            [&window](const Rectangle& rect, const Position& pos, const Style& style) {
                const sf::Color fillColor = style.fillColor;
                const sf::Color borderColor = style.borderColor;
                const float borderThickness = style.borderThickness;

                sf::RectangleShape r({
                    rect.width - 2 * borderThickness,
                    rect.height - 2 * borderThickness
                });
                r.setFillColor(fillColor);
                r.setOutlineColor(borderColor);
                r.setOutlineThickness(borderThickness);
                r.setPosition({
                    pos.x + borderThickness - rect.width / 2,
                    pos.y + borderThickness - rect.height / 2
                });

                window.draw(r);
            }
        );
}
