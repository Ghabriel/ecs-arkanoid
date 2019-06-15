#include "include.hpp"

#include "../../constants.hpp"

static void createPaddle(ecs::ComponentManager& world);
static void createBall(ecs::ComponentManager& world);
static void createBricks(ecs::ComponentManager& world);
static void createWalls(ecs::ComponentManager& world);

void useLevelLoadingSystem(ecs::ComponentManager& world) {
    createPaddle(world);
    createBall(world);
    createBricks(world);
    createWalls(world);
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
        Input { },
        Paddle { },
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
        Ball { },
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
    constexpr int BRICKS_PER_ROW = (MAX_X - BOARD_BORDER) / BRICK_WIDTH;

    for (int i = 0; i < BRICKS_PER_ROW; i++) {
        float x = BOARD_BORDER + i * BRICK_WIDTH + BRICK_WIDTH / 2;

        for (int j = 0; j < 6; j++) {
            float y = 100 + j * BRICK_HEIGHT + BRICK_HEIGHT / 2;

            world.createEntity(
                BounceCollision { },
                Position { x, y },
                Rectangle { BRICK_WIDTH, BRICK_HEIGHT },
                Visible { }
            );
        }
    }
}

void createWalls(ecs::ComponentManager& world) {
    using constants::WINDOW_WIDTH;
    using constants::WINDOW_HEIGHT;
    using constants::BOARD_BORDER;

    // Top
    world.createEntity(
        BounceCollision { },
        Position { WINDOW_WIDTH / 2, BOARD_BORDER / 2 },
        Rectangle { WINDOW_WIDTH, BOARD_BORDER },
        Visible { }
    );

    // Left
    world.createEntity(
        BounceCollision { },
        Position { BOARD_BORDER / 2, WINDOW_HEIGHT / 2 },
        Rectangle { BOARD_BORDER, WINDOW_HEIGHT },
        Visible { }
    );

    // Right
    world.createEntity(
        BounceCollision { },
        Position { WINDOW_WIDTH - BOARD_BORDER / 2, WINDOW_HEIGHT / 2 },
        Rectangle { BOARD_BORDER, WINDOW_HEIGHT },
        Visible { }
    );

    // Bottom
    world.createEntity(
        BounceCollision { },
        Position { WINDOW_WIDTH / 2, 0 },
        Rectangle { WINDOW_WIDTH, BOARD_BORDER },
        Visible { }
    );
}
