#include "include.hpp"

#include "../../constants.hpp"

#include <iostream>

void useCollisionHandlerSystem(ecs::ComponentManager& world, ecs::Entity id) {
    std::cout << "Collision detected with " << id << '\n';
}
