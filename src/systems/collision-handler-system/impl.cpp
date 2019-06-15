#include "include.hpp"

#include "../../constants.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

#include <iostream>

static void handlePaddleCollision(ecs::ComponentManager&, ecs::Entity, ecs::Entity);

void useCollisionHandlerSystem(
    ecs::ComponentManager& world,
    ecs::Entity ballId,
    ecs::Entity objectId
) {
    if (world.hasComponent<Paddle>(objectId)) {
        handlePaddleCollision(world, ballId, objectId);
        return;
    }

    std::cout << "Collision detected with " << objectId << '\n';
}

void handlePaddleCollision(
    ecs::ComponentManager& world,
    ecs::Entity ballId,
    ecs::Entity paddleId
) {
    std::cout << "Collision detected with Paddle\n";
}
