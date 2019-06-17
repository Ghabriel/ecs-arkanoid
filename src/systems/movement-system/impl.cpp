#include "include.hpp"

void useMovementSystem(ecs::World& world, float elapsedTime) {
    world.findAll<Position>()
        .join<Velocity>()
        .forEach(
            [elapsedTime](Position& pos, const Velocity& v) {
                pos += v * elapsedTime;
            }
        );

    world.findAll<Link>()
        .join<Position>()
        .forEach(
            [&world](const Link& link, Position& pos) {
                ecs::Entity target = link.target;
                const Position& targetPos = world.getData<Position>(target);

                pos = targetPos + link.relativePosition;
            }
        );
}
