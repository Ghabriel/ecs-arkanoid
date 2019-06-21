#include "include.hpp"

#include <algorithm>
#include "../../helpers/aggregate-data.hpp"

static void resolveBallCollisions(ecs::World&, float);
static void resolveBallPaddleCollisions(
    ecs::World&,
    ecs::Entity,
    const CircleData&,
    const CircleData&
);
static void resolveBounceCollisions(
    ecs::World&,
    ecs::Entity,
    const CircleData&,
    const CircleData&
);
static void resolvePaddleCollisions(ecs::World&, float);
static bool collides(const CircleData&, const RectangleData&);
static bool collides(const RectangleData&, const Velocity&, const RectangleData&);

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
            const Velocity& v
        ) {
            Velocity velocity = v * elapsedTime;
            Position nextPositionX { ballPos.x + velocity.x, ballPos.y };
            Position nextPositionY { ballPos.x, ballPos.y + velocity.y };
            CircleData nextCircleDataX { c, nextPositionX };
            CircleData nextCircleDataY { c, nextPositionY };

            resolveBallPaddleCollisions(world, ballId, nextCircleDataX, nextCircleDataY);
            resolveBounceCollisions(world, ballId, nextCircleDataX, nextCircleDataY);
        });
}

void resolveBallPaddleCollisions(
    ecs::World& world,
    ecs::Entity ballId,
    const CircleData& nextCircleDataX,
    const CircleData& nextCircleDataY
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
    const CircleData& nextCircleDataY
) {
    std::vector<metadata::CollisionData> collidedObjects;

    world.findAll<BounceCollision>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, &collidedObjects, &nextCircleDataX, &nextCircleDataY](
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
            const Position& paddlePos,
            const Velocity& paddleVelocity
        ) {
            RectangleData paddle { paddleBody, paddlePos };
            Velocity velocity = paddleVelocity * elapsedTime;

            world.findAll<Wall>()
                .join<Rectangle>()
                .join<Position>()
                .forEach([&](
                    ecs::Entity wallId,
                    const Rectangle& r,
                    const Position& rectPos
                ) {
                    RectangleData wall { r, rectPos };

                    if (collides(paddle, velocity, wall)) {
                        world.notify<PaddleWallCollisionListener>(paddleId, wallId);
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

bool collides(
    const RectangleData& paddle,
    const Velocity& paddleVelocity,
    const RectangleData& wall
) {
    auto leftX = [](auto& data) { return data.position.x - data.body.width / 2; };
    auto rightX = [](auto& data) { return data.position.x + data.body.width / 2; };
    auto topY = [](auto& data) { return data.position.y - data.body.height / 2; };
    auto bottomY = [](auto& data) { return data.position.y + data.body.height / 2; };

    float distanceLeft = rightX(wall) - leftX(paddle);
    float distanceRight = leftX(wall) - rightX(paddle);
    float distanceTop = bottomY(wall) - topY(paddle);
    float distanceBottom = topY(wall) - bottomY(paddle);

    float checkLeft = distanceLeft <= paddleVelocity.x;
    float checkRight = paddleVelocity.x <= distanceRight;
    float checkTop = distanceTop <= paddleVelocity.y;
    float checkBottom = paddleVelocity.y <= distanceBottom;

    return !checkLeft && !checkRight && !checkTop && !checkBottom;
}
