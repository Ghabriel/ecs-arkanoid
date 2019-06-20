#include "include.hpp"

#include <bitset>
#include "../../constants.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

#include <iostream>

static void handlePaddleCollision(ecs::World&, ecs::Entity, ecs::Entity);
static void handleBrickCollision(ecs::World&, ecs::Entity, ecs::Entity);

void usePaddleCollisionHandlerSystem(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity paddleId
) {
    handlePaddleCollision(world, ballId, paddleId);
}

void useBounceCollisionHandlerSystem(
    ecs::World& world,
    ecs::Entity ballId,
    const metadata::MultiCollisionData& collisions
) {
    bool collidesInX = false;
    bool collidesInY = false;

    for (const metadata::CollisionData& collisionData : collisions) {
        ecs::Entity objectId = collisionData.objectId;
        if (world.hasComponent<Brick>(objectId)) {
            handleBrickCollision(world, ballId, objectId);
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

void handlePaddleCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity paddleId
) {
    std::cout << "Collision detected with Paddle\n";
    const Position& ballPos = world.getData<Position>(ballId);
    const Position& paddlePos = world.getData<Position>(paddleId);
    world.getData<Velocity>(ballId) = getBallNewVelocity(ballPos, paddlePos);
}

void handleBrickCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity brickId
) {
    std::cout << "Collision detected with brick " << brickId << '\n';
    world.deleteEntity(brickId);
}
