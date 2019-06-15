#include "include.hpp"

#include <algorithm>
#include <bitset>
#include "../../constants.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

struct CircleData {
    const Circle& body;
    const Position& position;
};

struct RectangleData {
    const Rectangle& body;
    const Position& position;
};

namespace Axis {
    constexpr size_t X = 0;
    constexpr size_t Y = 1;
}

static void resolveCollisions(ecs::ComponentManager&, float);
static void resolvePaddleCollisions(
    ecs::ComponentManager&,
    ecs::Entity,
    const Position&,
    const CircleData&,
    const CircleData&,
    Velocity&
);
static void resolveBounceCollisions(
    ecs::ComponentManager&,
    ecs::Entity,
    const CircleData&,
    const CircleData&,
    Velocity&
);
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
        .forEach([&world, elapsedTime](
            ecs::Entity ballId,
            const Circle& c,
            const Position& ballPos,
            Velocity& v
        ) {
            Velocity velocity { v.x * elapsedTime, v.y * elapsedTime };
            Position nextPositionX { ballPos.x + velocity.x, ballPos.y };
            Position nextPositionY { ballPos.x, ballPos.y + velocity.y };
            CircleData nextCircleDataX { c, nextPositionX };
            CircleData nextCircleDataY { c, nextPositionY };

            resolvePaddleCollisions(
                world,
                ballId,
                ballPos,
                nextCircleDataX,
                nextCircleDataY,
                v
            );

            resolveBounceCollisions(
                world,
                ballId,
                nextCircleDataX,
                nextCircleDataY,
                v
            );
        });
}

void resolvePaddleCollisions(
    ecs::ComponentManager& world,
    ecs::Entity ballId,
    const Position& ballPos,
    const CircleData& nextCircleDataX,
    const CircleData& nextCircleDataY,
    Velocity& ballVelocity
) {
    world.findAll<Paddle>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&](
            ecs::Entity paddleId,
            const Rectangle& r,
            const Position& paddlePos
        ) {
            RectangleData rectangle { r, paddlePos };
            std::bitset<2> willCollide;
            willCollide.set(Axis::X, collides(nextCircleDataX, rectangle));
            willCollide.set(Axis::Y, collides(nextCircleDataY, rectangle));

            if (willCollide.any()) {
                ballVelocity = getBallNewVelocity(ballPos, paddlePos);
                world.notify<BallCollisionListener>(ballId, paddleId);
            }
        });
}

void resolveBounceCollisions(
    ecs::ComponentManager& world,
    ecs::Entity ballId,
    const CircleData& nextCircleDataX,
    const CircleData& nextCircleDataY,
    Velocity& ballVelocity
) {
    std::bitset<2> collisionAxis;

    world.findAll<BounceCollision>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&](
            ecs::Entity objectId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData rectangle { r, rectPos };
            std::bitset<2> willCollide;
            willCollide.set(Axis::X, collides(nextCircleDataX, rectangle));
            willCollide.set(Axis::Y, collides(nextCircleDataY, rectangle));

            collisionAxis |= willCollide;

            if (willCollide.any()) {
                world.notify<BallCollisionListener>(ballId, objectId);
            }
        });

    if (collisionAxis[Axis::X]) {
        ballVelocity.x *= -1;
    }

    if (collisionAxis[Axis::Y]) {
        ballVelocity.y *= -1;
    }
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
