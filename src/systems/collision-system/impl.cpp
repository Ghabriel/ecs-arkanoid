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

namespace Axis {
    constexpr size_t X = 0;
    constexpr size_t Y = 1;
}

static void resolveBallCollisions(ecs::World&, float);
static void resolveBallPaddleCollisions(
    ecs::World&,
    ecs::Entity,
    const CircleData&,
    const CircleData&,
    const Velocity&
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
static bool resolveCollision(const Rectangle&, Position&, const Velocity&, const RectangleData&);

void useCollisionSystem(ecs::World& world, float elapsedTime) {
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

            resolveBallPaddleCollisions(world, ballId, nextCircleDataX, nextCircleDataY, v);
            resolveBounceCollisions(world, ballId, nextCircleDataX, nextCircleDataY, v);
        });
}

void resolveBallPaddleCollisions(
    ecs::World& world,
    ecs::Entity ballId,
    const CircleData& nextCircleDataX,
    const CircleData& nextCircleDataY,
    const Velocity& ballVelocity
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
            bool collidesInX = collides(nextCircleDataX, rectangle);
            bool collidesInY = collides(nextCircleDataY, rectangle);

            if (collidesInX || collidesInY) {
                world.notify<BallPaddleCollisionListener>(ballId, paddleId);
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
    std::vector<metadata::CollisionData> collidedObjects;

    world.findAll<BounceCollision>()
        .join<Rectangle>()
        .join<Position>()
        .mutatingForEach([&world, &collidedObjects, &nextCircleDataX, &nextCircleDataY](
            ecs::Entity objectId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData rectangle { r, rectPos };
            bool collidesInX = collides(nextCircleDataX, rectangle);
            bool collidesInY = collides(nextCircleDataY, rectangle);

            if (collidesInX || collidesInY) {
                collidedObjects.push_back({ objectId, collidesInX, collidesInY });
            }
        });

    world.notify<BallObjectsCollisionListener>(ballId, collidedObjects);
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
                    bool collided = resolveCollision(paddleBody, paddlePos, velocity, wall);

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

bool resolveCollision(
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

    float distanceLeft = rightX(wall) - leftX(paddle);
    float distanceRight = leftX(wall) - rightX(paddle);
    float distanceTop = bottomY(wall) - topY(paddle);
    float distanceBottom = topY(wall) - bottomY(paddle);

    float checkLeft = distanceLeft <= paddleVelocity.x;
    float checkRight = paddleVelocity.x <= distanceRight;
    float checkTop = distanceTop <= paddleVelocity.y;
    float checkBottom = paddleVelocity.y <= distanceBottom;

    if (checkLeft || checkRight || checkTop || checkBottom) {
        return false;
    }

    std::array<float, 4> ts {
        distanceLeft / paddleVelocity.x,
        distanceRight / paddleVelocity.x,
        distanceTop / paddleVelocity.y,
        distanceBottom / paddleVelocity.y
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
