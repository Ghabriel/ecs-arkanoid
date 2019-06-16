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

static void resolveCollisions(ecs::World&, float);
static void resolveBallCollisions(ecs::World&, float);
static void resolveBallPaddleCollisions(
    ecs::World&,
    ecs::Entity,
    const Position&,
    const CircleData&,
    const CircleData&,
    Velocity&
);
static void resolveBounceCollisions(
    ecs::World&,
    ecs::Entity,
    const CircleData&,
    const CircleData&,
    Velocity&
);
static void resolvePaddleCollisions(ecs::World&, float);
static bool collides(const CircleData&, const RectangleData&);
static bool solveCollision(const Rectangle&, Position&, const Velocity&, const RectangleData&);

void useMovementSystem(ecs::World& world, float elapsedTime) {
    resolveCollisions(world, elapsedTime);

    world.findAll<Position>()
        .join<Velocity>()
        .forEach(
            [elapsedTime](Position& pos, const Velocity& v) {
                pos += v * elapsedTime;
            }
        );

    world.findAll<Link>()
        .join<Position>()
        .forEach(
            [&world, elapsedTime](const Link& link, Position& pos) {
                ecs::Entity target = link.target;
                const Position& targetPos = world.getData<Position>(target);

                pos = targetPos + link.relativePosition;
            }
        );
}

void resolveCollisions(ecs::World& world, float elapsedTime) {
    resolveBallCollisions(world, elapsedTime);
    resolvePaddleCollisions(world, elapsedTime);
}

void resolveBallCollisions(ecs::World& world, float elapsedTime) {
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
            Velocity velocity = v * elapsedTime;
            Position nextPositionX { ballPos.x + velocity.x, ballPos.y };
            Position nextPositionY { ballPos.x, ballPos.y + velocity.y };
            CircleData nextCircleDataX { c, nextPositionX };
            CircleData nextCircleDataY { c, nextPositionY };

            resolveBallPaddleCollisions(world, ballId, ballPos, nextCircleDataX, nextCircleDataY, v);
            resolveBounceCollisions(world, ballId, nextCircleDataX, nextCircleDataY, v);
        });
}

void resolveBallPaddleCollisions(
    ecs::World& world,
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
    ecs::World& world,
    ecs::Entity ballId,
    const CircleData& nextCircleDataX,
    const CircleData& nextCircleDataY,
    Velocity& ballVelocity
) {
    std::bitset<2> collisionAxis;

    world.findAll<BounceCollision>()
        .join<Rectangle>()
        .join<Position>()
        .mutatingForEach([&](
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

void resolvePaddleCollisions(ecs::World& world, float elapsedTime) {
    world.findAll<Paddle>()
        .join<Rectangle>()
        .join<Position>()
        .join<Velocity>()
        .forEach([&world, elapsedTime](
            ecs::Entity paddleId,
            const Rectangle& paddleBody,
            Position& paddlePos,
            Velocity& paddleVelocity
        ) {
            Velocity velocity = paddleVelocity * elapsedTime;

            world.findAll<Wall>()
                .join<Rectangle>()
                .join<Position>()
                .forEach([&](
                    ecs::Entity objectId,
                    const Rectangle& r,
                    const Position& rectPos
                ) {
                    RectangleData wall { r, rectPos };
                    bool collided = solveCollision(paddleBody, paddlePos, velocity, wall);

                    if (collided) {
                        world.removeComponent<Velocity>(paddleId);
                    }
                });
        });
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

bool solveCollision(
    const Rectangle& paddleBody,
    Position& paddlePos,
    const Velocity& paddleVelocity,
    const RectangleData& wall
) {
    auto leftX = [](auto& data) { return data.position.x - data.body.width / 2; };
    auto rightX = [](auto& data) { return data.position.x + data.body.width / 2; };
    auto topY = [](auto& data) { return data.position.y - data.body.height / 2; };
    auto bottomY = [](auto& data) { return data.position.y + data.body.height / 2; };

    RectangleData paddle { paddleBody, paddlePos };
    RectangleData futurePaddle { paddle.body, paddlePos + paddleVelocity };

    float checkLeft = rightX(wall) <= leftX(futurePaddle);
    float checkRight = rightX(futurePaddle) <= leftX(wall);
    float checkTop = bottomY(wall) <= topY(futurePaddle);
    float checkBottom = bottomY(futurePaddle) <= topY(wall);

    if (checkLeft || checkRight || checkTop || checkBottom) {
        return false;
    }

    std::array<float, 4> ts {
        (rightX(wall) - leftX(paddle)) / paddleVelocity.x,
        (leftX(wall) - rightX(paddle)) / paddleVelocity.x,
        (bottomY(wall) - topY(paddle)) / paddleVelocity.y,
        (topY(wall) - bottomY(paddle)) / paddleVelocity.y
    };

    float minValidT = 1;

    for (float t : ts) {
        if (t >= 0 && t < minValidT) {
            minValidT = t;
        }
    }

    paddlePos += paddleVelocity * minValidT;

    return true;
}
