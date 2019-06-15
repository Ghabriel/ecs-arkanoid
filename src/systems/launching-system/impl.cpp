#include "include.hpp"

#include "../../helpers/ball-paddle-contact.hpp"

void useLaunchingSystem(ecs::ComponentManager& world) {
    ecs::Entity paddleId = world.unique<Paddle>();
    const Position& paddlePos = world.getData<Position>(paddleId);

    world.query<Ball, Position>(
        [&world, &paddlePos](ecs::Entity ballId, Ball, const Position& pos) {
            world.replaceComponent(ballId, getBallNewVelocity(pos, paddlePos));
        }
    );
}
