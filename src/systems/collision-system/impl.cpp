#include "include.hpp"

#include <algorithm>
#include "../../helpers/aggregate-data.hpp"

static void detectBallCollisions(ecs::World&, float);
static void detectBallPaddleCollisions(
    ecs::World&,
    ecs::Entity,
    const CircleData&,
    const CircleData&
);
static void detectBounceCollisions(
    ecs::World&,
    ecs::Entity,
    const CircleData&,
    const CircleData&
);
static void detectPaddleCollisions(ecs::World&, float);
static void detectPaddlePowerUpCollisions(ecs::World&, ecs::Entity, const RectangleData&);
static void detectPaddleWallCollisions(
    ecs::World&,
    ecs::Entity,
    const RectangleData&,
    const Velocity&
);
static bool collides(const CircleData&, const RectangleData&);
static bool collides(const RectangleData&, const Velocity&, const RectangleData&);

void useCollisionSystem(ecs::World& world, float elapsedTime) {
    detectBallCollisions(world, elapsedTime);
    detectPaddleCollisions(world, elapsedTime);
}

void detectBallCollisions(ecs::World& world, float elapsedTime) {
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
            CircleData nextBallDataX { c, nextPositionX };
            CircleData nextBallDataY { c, nextPositionY };

            detectBallPaddleCollisions(world, ballId, nextBallDataX, nextBallDataY);
            detectBounceCollisions(world, ballId, nextBallDataX, nextBallDataY);
        });
}

void detectBallPaddleCollisions(
    ecs::World& world,
    ecs::Entity ballId,
    const CircleData& nextBallDataX,
    const CircleData& nextBallDataY
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
            bool collidesInX = collides(nextBallDataX, rectangle);
            bool collidesInY = collides(nextBallDataY, rectangle);

            if (collidesInX || collidesInY) {
                world.notify<BallPaddleCollisionListener>(ballId, paddleId);
            }
        });
}

void detectBounceCollisions(
    ecs::World& world,
    ecs::Entity ballId,
    const CircleData& nextBallDataX,
    const CircleData& nextBallDataY
) {
    std::vector<metadata::CollisionData> collidedObjects;

    world.findAll<BounceCollision>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, &collidedObjects, &nextBallDataX, &nextBallDataY](
            ecs::Entity objectId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData rectangle { r, rectPos };
            bool collidesInX = collides(nextBallDataX, rectangle);
            bool collidesInY = collides(nextBallDataY, rectangle);

            if (collidesInX || collidesInY) {
                collidedObjects.push_back({ objectId, collidesInX, collidesInY });
            }
        });

    world.notify<BallObjectsCollisionListener>(ballId, collidedObjects);
}

void detectPaddleCollisions(ecs::World& world, float elapsedTime) {
    world.findAll<Paddle>()
        .join<Rectangle>()
        .join<Position>()
        .join<Velocity>()
        .forEach([&world, elapsedTime](
            ecs::Entity paddleId,
            const Rectangle& paddleBody,
            const Position& paddlePos,
            const Velocity& v
        ) {
            RectangleData paddle { paddleBody, paddlePos };
            Velocity paddleVelocity = v * elapsedTime;

            detectPaddlePowerUpCollisions(world, paddleId, paddle);
            detectPaddleWallCollisions(world, paddleId, paddle, paddleVelocity);
        });
}

void detectPaddlePowerUpCollisions(
    ecs::World& world,
    ecs::Entity paddleId,
    const RectangleData& paddle
) {
    world.findAll<PowerUp>()
        .join<Circle>()
        .join<Position>()
        .join<Velocity>()
        .forEach([&world, paddleId, &paddle](
            ecs::Entity powerUpId,
            const Circle& powerUpBody,
            const Position& powerUpPos,
            const Velocity& powerUpVelocity
        ) {
            CircleData powerUp { powerUpBody, powerUpPos };

            if (collides(powerUp, paddle)) {
                world.notify<PaddlePowerUpCollisionListener>(paddleId, powerUpId);
            }
        });
}

void detectPaddleWallCollisions(
    ecs::World& world,
    ecs::Entity paddleId,
    const RectangleData& paddle,
    const Velocity& paddleVelocity
) {
    world.findAll<Wall>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, &paddleId, &paddle, &paddleVelocity](
            ecs::Entity wallId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData wall { r, rectPos };

            if (collides(paddle, paddleVelocity, wall)) {
                world.notify<PaddleWallCollisionListener>(paddleId, wallId);
            }
        });
}

bool collides(const CircleData& c, const RectangleData& r) {
    const auto& [circle, circlePos] = c;
    const auto& [rectangle, rectPos] = r;

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
    bool checkLeft = (wall.rightX() - paddle.leftX()) > paddleVelocity.x;
    bool checkRight = paddleVelocity.x > (wall.leftX() - paddle.rightX());
    bool checkTop = (wall.bottomY() - paddle.topY()) > paddleVelocity.y;
    bool checkBottom = paddleVelocity.y > (wall.topY() - paddle.bottomY());

    return checkLeft && checkRight && checkTop && checkBottom;
}
