#include "level-loading-system.hpp"

#include "../constants.hpp"

void createPaddle(ecs::ComponentManager& world);
void createBall(ecs::ComponentManager& world);
void createBricks(ecs::ComponentManager& world);

void useLevelLoadingSystem(ecs::ComponentManager& world) {
    createPaddle(world);
    createBall(world);
    createBricks(world);
}

void createPaddle(ecs::ComponentManager& world) {
    using constants::WINDOW_WIDTH;
    using constants::WINDOW_HEIGHT;
    using constants::PADDLE_WIDTH;
    using constants::PADDLE_HEIGHT;
    using constants::PADDLE_BORDER_DISTANCE;

    constexpr float x = WINDOW_WIDTH / 2;
    constexpr float y = WINDOW_HEIGHT - PADDLE_BORDER_DISTANCE - PADDLE_HEIGHT / 2;

    world.createEntity(
        Position { x, y },
        Rectangle { PADDLE_WIDTH, PADDLE_HEIGHT },
        Visible { }
    );
}

void createBall(ecs::ComponentManager& world) {
    using constants::WINDOW_WIDTH;
    using constants::WINDOW_HEIGHT;
    using constants::BALL_RADIUS;
    using constants::PADDLE_HEIGHT;
    using constants::PADDLE_BORDER_DISTANCE;

    constexpr float x = WINDOW_WIDTH / 2;
    constexpr float y = WINDOW_HEIGHT - PADDLE_BORDER_DISTANCE - PADDLE_HEIGHT - BALL_RADIUS;

    world.createEntity(
        Circle { BALL_RADIUS },
        Position { x, y },
        Visible { }
    );
}

void createBricks(ecs::ComponentManager& world) {
    using constants::WINDOW_WIDTH;
    using constants::BRICK_WIDTH;
    using constants::BRICK_HEIGHT;
    using constants::BOARD_BORDER;

    constexpr float MAX_X = WINDOW_WIDTH - BOARD_BORDER;
    for (int x = BOARD_BORDER; x <= MAX_X; x += BRICK_WIDTH) {
        for (int i = 0; i < 6; i++) {
            float y = 100 + i * BRICK_HEIGHT;

            world.createEntity(
                Position { static_cast<float>(x), y },
                Rectangle { BRICK_WIDTH, BRICK_HEIGHT },
                Visible { }
            );
        }
    }
}
