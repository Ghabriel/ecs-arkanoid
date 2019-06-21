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
    std::vector<ecs::Entity> collidedPaddles;

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
                collidedPaddles.push_back({ paddleId });
            }
        });

    if (!collidedPaddles.empty()) {
        world.notify<CollisionListener<Ball, Paddle>>(ballId, collidedPaddles);
    }
}

void detectBounceCollisions(
    ecs::World& world,
    ecs::Entity ballId,
    const CircleData& nextBallDataX,
    const CircleData& nextBallDataY
) {
    std::vector<metadata::RectCollisionData> collidedBricks;

    world.findAll<Brick>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, &collidedBricks, &nextBallDataX, &nextBallDataY](
            ecs::Entity objectId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData rectangle { r, rectPos };
            bool collidesInX = collides(nextBallDataX, rectangle);
            bool collidesInY = collides(nextBallDataY, rectangle);

            if (collidesInX || collidesInY) {
                collidedBricks.push_back({ objectId, collidesInX, collidesInY });
            }
        });

    if (!collidedBricks.empty()) {
        world.notify<CollisionListener<Ball, Brick>>(ballId, collidedBricks);
    }



    std::vector<metadata::RectCollisionData> collidedWalls;

    world.findAll<Wall>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, &collidedWalls, &nextBallDataX, &nextBallDataY](
            ecs::Entity objectId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData rectangle { r, rectPos };
            bool collidesInX = collides(nextBallDataX, rectangle);
            bool collidesInY = collides(nextBallDataY, rectangle);

            if (collidesInX || collidesInY) {
                collidedWalls.push_back({ objectId, collidesInX, collidesInY });
            }
        });

    if (!collidedWalls.empty()) {
        world.notify<CollisionListener<Ball, Wall>>(ballId, collidedWalls);
    }
}

void detectPaddleCollisions(ecs::World& world, float elapsedTime) {
    world.findAll<Paddle>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, elapsedTime](
            ecs::Entity paddleId,
            const Rectangle& paddleBody,
            const Position& paddlePos
        ) {
            RectangleData paddle { paddleBody, paddlePos };

            detectPaddlePowerUpCollisions(world, paddleId, paddle);

            if (world.hasComponent<Velocity>(paddleId)) {
                const Velocity& v = world.getData<Velocity>(paddleId);
                Velocity paddleVelocity = v * elapsedTime;

                detectPaddleWallCollisions(world, paddleId, paddle, paddleVelocity);
            }
        });
}

void detectPaddlePowerUpCollisions(
    ecs::World& world,
    ecs::Entity paddleId,
    const RectangleData& paddle
) {
    std::vector<ecs::Entity> collidedPowerUps;

    world.findAll<PowerUp>()
        .join<Circle>()
        .join<Position>()
        .join<Velocity>()
        .forEach([&world, paddleId, &paddle, &collidedPowerUps](
            ecs::Entity powerUpId,
            const Circle& powerUpBody,
            const Position& powerUpPos,
            const Velocity& powerUpVelocity
        ) {
            CircleData powerUp { powerUpBody, powerUpPos };

            if (collides(powerUp, paddle)) {
                collidedPowerUps.push_back({ powerUpId });
            }
        });

    if (!collidedPowerUps.empty()) {
        world.notify<CollisionListener<Paddle, PowerUp>>(paddleId, collidedPowerUps);
    }
}

void detectPaddleWallCollisions(
    ecs::World& world,
    ecs::Entity paddleId,
    const RectangleData& paddle,
    const Velocity& paddleVelocity
) {
    std::vector<ecs::Entity> collidedWalls;

    world.findAll<Wall>()
        .join<Rectangle>()
        .join<Position>()
        .forEach([&world, &paddleId, &paddle, &paddleVelocity, &collidedWalls](
            ecs::Entity wallId,
            const Rectangle& r,
            const Position& rectPos
        ) {
            RectangleData wall { r, rectPos };

            if (collides(paddle, paddleVelocity, wall)) {
                collidedWalls.push_back({ wallId });
            }
        });

    if (!collidedWalls.empty()) {
        world.notify<CollisionListener<Paddle, Wall>>(paddleId, collidedWalls);
    }
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
