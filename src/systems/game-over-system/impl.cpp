#include "include.hpp"

#include "../../constants.hpp"

void useGameOverSystem(ecs::World& world) {
    bool hasBallsInPlay = false;

    world.findAll<Ball>()
        .join<Position>()
        .mutatingForEach(
            [&world, &hasBallsInPlay](ecs::Entity ballId, Position& pos) {
                using constants::WINDOW_HEIGHT;

                if (pos.y >= WINDOW_HEIGHT) {
                    world.deleteEntity(ballId);
                } else {
                    hasBallsInPlay = true;
                }
            }
        );

    if (!hasBallsInPlay) {
        world.notify<GameOverListener>();
    }
}
