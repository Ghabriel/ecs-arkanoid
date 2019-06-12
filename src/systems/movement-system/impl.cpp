#include "include.hpp"

void useMovementSystem(ecs::ComponentManager& world, float elapsedTime) {
    world.findAll<Position>()
        .join<Velocity>()
        .forEach(
            [elapsedTime](Position& pos, const Velocity& v) {
                pos.x += v.x * elapsedTime;
                pos.y += v.y * elapsedTime;
            }
        );
}
