#include "include.hpp"

#include "../../helpers/ball-paddle-contact.hpp"

void useLaunchingSystem(ecs::ComponentManager& world) {
    ecs::Entity paddleId = world.unique<Paddle>();
    const Position& paddlePos = world.getData<Position>(paddleId);

    world.findAll<Ball>()
        .join<Position>()
        .forEach(
            [&world, &paddlePos](ecs::Entity ballId, const Position& pos) {
                world.replaceComponent(ballId, getBallNewVelocity(pos, paddlePos));
            }
        );
}
