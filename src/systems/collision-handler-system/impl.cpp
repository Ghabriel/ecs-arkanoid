#include "include.hpp"

#include "../../constants.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

#include <iostream>

static void handlePaddleCollision(ecs::World&, ecs::Entity, ecs::Entity);
static void handleBrickCollision(ecs::World&, ecs::Entity, ecs::Entity);

void useCollisionHandlerSystem(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity objectId
) {
    if (world.hasComponent<Paddle>(objectId)) {
        handlePaddleCollision(world, ballId, objectId);
        return;
    }

    if (world.hasComponent<Brick>(objectId)) {
        handleBrickCollision(world, ballId, objectId);
        return;
    }

    std::cout << "Collision detected with wall " << objectId << '\n';
}

void handlePaddleCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity paddleId
) {
    std::cout << "Collision detected with Paddle\n";
}

void handleBrickCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity brickId
) {
    std::cout << "Collision detected with brick " << brickId << '\n';
    world.deleteEntity(brickId);
}
