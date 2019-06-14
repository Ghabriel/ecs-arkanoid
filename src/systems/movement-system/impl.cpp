#include "include.hpp"

#include <algorithm>
#include "../../constants.hpp"

struct CircleData {
    const Circle& body;
    const Position& position;
};

struct RectangleData {
    const Rectangle& body;
    const Position& position;
};

static void resolveCollisions(ecs::ComponentManager&, float);
static bool collides(const CircleData&, const RectangleData&);

void useMovementSystem(ecs::ComponentManager& world, float elapsedTime) {
    resolveCollisions(world, elapsedTime);

    world.findAll<Position>()
        .join<Velocity>()
        .forEach(
            [elapsedTime](Position& pos, const Velocity& v) {
                pos.x += v.x * elapsedTime;
                pos.y += v.y * elapsedTime;
            }
        );
}

void resolveCollisions(ecs::ComponentManager& world, float elapsedTime) {
    world.findAll<Ball>()
        .join<Circle>()
        .join<Position>()
        .join<Velocity>()
        .forEach([&world, elapsedTime](const Circle& c, const Position& ballPos, Velocity& v) {
            Velocity velocity { v.x * elapsedTime, v.y * elapsedTime };
            Position nextPositionX { ballPos.x + velocity.x, ballPos.y };
            Position nextPositionY { ballPos.x, ballPos.y + velocity.y };
            bool changedDirectionX = false;
            bool changedDirectionY = false;

            world.findAll<Rectangle>()
                .join<Position>()
                .forEach([&](ecs::Entity id, const Rectangle& r, const Position& rectPos) {
                    RectangleData rectangle { r, rectPos };
                    bool willCollide = false;

                    if (collides(CircleData { c, nextPositionX }, rectangle)) {
                        willCollide = true;

                        if (!changedDirectionX) {
                            v.x *= -1;
                            changedDirectionX = true;
                        }
                    }

                    if (collides(CircleData { c, nextPositionY }, rectangle)) {
                        willCollide = true;

                        if (!changedDirectionY) {
                            v.y *= -1;
                            changedDirectionY = true;
                        }
                    }

                    if (willCollide) {
                        world.notify<BallCollisionListener>(id);
                    }
                });
            }
        );
}

bool collides(const CircleData& c, const RectangleData& r) {
    const auto& [circle, circlePos] = c;
    const auto& [rectangle, rectPos] = r;
    //{}

    float rectLeftX = rectPos.x - rectangle.width / 2;
    float rectRightX = rectPos.x + rectangle.width / 2;
    float rectTopY = rectPos.y - rectangle.height / 2;
    float rectBottomY = rectPos.y + rectangle.height / 2;

    float closestX = std::clamp(circlePos.x, rectLeftX, rectRightX);
    float closestY = std::clamp(circlePos.y, rectTopY, rectBottomY);

    float dx = circlePos.x - closestX;
    float dy = circlePos.y - closestY;

    return (dx * dx) + (dy * dy) < (circle.radius * circle.radius);
}
