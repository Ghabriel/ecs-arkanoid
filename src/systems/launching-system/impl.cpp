#include "include.hpp"

#include <cmath>
#include "../../constants.hpp"

void useLaunchingSystem(ecs::ComponentManager& world) {
    ecs::Entity paddleId = world.unique<Paddle>();
    const Position& paddlePos = world.getData<Position>(paddleId);

    world.query<Ball, Position>(
        [&world, &paddlePos](ecs::Entity ballId, Ball, const Position& pos) {
            float dx = pos.x - paddlePos.x;
            float dy = pos.y - paddlePos.y;
            float angle = std::atan2(dy, dx);

            using constants::BALL_VELOCITY;

            world.addComponent<Velocity>(ballId, {
                BALL_VELOCITY * std::cos(angle),
                BALL_VELOCITY * std::sin(angle)
            });
        }
    );
}
