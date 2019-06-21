#include "include.hpp"

#include <bitset>
#include "../../constants.hpp"
#include "../../helpers/aggregate-data.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

#include <iostream>

static void handleBallPaddleCollision(ecs::World&, ecs::Entity, ecs::Entity);
static void handleBallBrickCollision(ecs::World&, ecs::Entity, ecs::Entity);

void useBallPaddleCollisionSystem(ecs::World& world, ecs::Entity ballId, ecs::Entity paddleId) {
    handleBallPaddleCollision(world, ballId, paddleId);
}

void useBounceCollisionSystem(
    ecs::World& world,
    ecs::Entity ballId,
    const metadata::MultiCollisionData& collisions
) {
    bool collidesInX = false;
    bool collidesInY = false;

    for (const metadata::CollisionData& collisionData : collisions) {
        ecs::Entity objectId = collisionData.objectId;
        if (world.hasComponent<Brick>(objectId)) {
            handleBallBrickCollision(world, ballId, objectId);
        } else {
            std::cout << "Collision detected with wall " << objectId << '\n';
        }

        collidesInX = collidesInX || collisionData.collidesInX;
        collidesInY = collidesInY || collisionData.collidesInY;
    }

    Velocity& ballVelocity = world.getData<Velocity>(ballId);

    if (collidesInX) {
        ballVelocity.x *= -1;
    }

    if (collidesInY) {
        ballVelocity.y *= -1;
    }
}

void usePaddleWallCollisionSystem(ecs::World& world, ecs::Entity paddleId, ecs::Entity wallId) {
    std::cout << "Collision detected between Paddle and Wall " << wallId << "\n";

    Rectangle& paddleBody = world.getData<Rectangle>(paddleId);
    Position& paddlePos = world.getData<Position>(paddleId);
    Velocity& paddleVelocity = world.getData<Velocity>(paddleId);
    RectangleData paddle { paddleBody, paddlePos };

    const Rectangle& wallBody = world.getData<Rectangle>(wallId);
    const Position& wallPos = world.getData<Position>(wallId);
    RectangleData wall { wallBody, wallPos };

    std::array<float, 4> ts {
        (wall.rightX() - paddle.leftX()) / paddleVelocity.x,
        (wall.leftX() - paddle.rightX()) / paddleVelocity.x,
        (wall.bottomY() - paddle.topY()) / paddleVelocity.y,
        (wall.topY() - paddle.bottomY()) / paddleVelocity.y
    };

    float minValidT = 1;

    for (float t : ts) {
        if (t >= 0 && t < minValidT) {
            minValidT = t;
        }
    }

    paddlePos += paddleVelocity * minValidT;
    world.removeComponent<Velocity>(paddleId);
}

void handleBallPaddleCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity paddleId
) {
    std::cout << "Collision detected with Paddle\n";
    const Position& ballPos = world.getData<Position>(ballId);
    const Position& paddlePos = world.getData<Position>(paddleId);
    world.getData<Velocity>(ballId) = getBallNewVelocity(ballPos, paddlePos);
}

void handleBallBrickCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity brickId
) {
    std::cout << "Collision detected with brick " << brickId << '\n';
    world.deleteEntity(brickId);
}
