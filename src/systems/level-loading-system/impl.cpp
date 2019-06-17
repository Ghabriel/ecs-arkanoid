#include "include.hpp"

#include "../../constants.hpp"

static void createPaddle(ecs::World& world);
static void createBall(ecs::World& world);
static void createBricks(ecs::World& world);
static void createWalls(ecs::World& world);

void useLevelLoadingSystem(ecs::World& world) {
    createPaddle(world);
    createBall(world);
    createBricks(world);
    createWalls(world);
}

void createPaddle(ecs::World& world) {
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
        Style { sf::Color::White, sf::Color::Blue, 1 },
        Visible { }
    );
}

void createBall(ecs::World& world) {
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
        Style { sf::Color::Blue, sf::Color::Green, 2 },
        Visible { }
    );
}

void createBricks(ecs::World& world) {
    using constants::WINDOW_WIDTH;
    using constants::BRICK_WIDTH;
    using constants::BRICK_HEIGHT;
    using constants::BOARD_BORDER;

    constexpr float MAX_X = WINDOW_WIDTH - BOARD_BORDER;
    constexpr int BRICKS_PER_ROW = (MAX_X - BOARD_BORDER) / BRICK_WIDTH;

    std::array<Style, 6> styles {
        Style { sf::Color::White, sf::Color::Blue, 1 },
        Style { sf::Color::Green, sf::Color::Blue, 1 },
        Style { sf::Color::White, sf::Color::Blue, 1 },
        Style { sf::Color::Green, sf::Color::Blue, 1 },
        Style { sf::Color::White, sf::Color::Blue, 1 },
        Style { sf::Color::Green, sf::Color::Blue, 1 }
    };

    for (int i = 0; i < BRICKS_PER_ROW; i++) {
        float x = BOARD_BORDER + i * BRICK_WIDTH + BRICK_WIDTH / 2;

        for (int j = 0; j < 6; j++) {
            float y = 100 + j * BRICK_HEIGHT + BRICK_HEIGHT / 2;

            world.createEntity(
                BounceCollision { },
                Brick { },
                Position { x, y },
                Rectangle { BRICK_WIDTH, BRICK_HEIGHT },
                styles[j],
                Visible { }
            );
        }
    }
}

void createWalls(ecs::World& world) {
    using constants::WINDOW_WIDTH;
    using constants::WINDOW_HEIGHT;
    using constants::BOARD_BORDER;

    Style style { sf::Color::White, sf::Color::White, 1 };

    // Top
    world.createEntity(
        BounceCollision { },
        Position { WINDOW_WIDTH / 2, BOARD_BORDER / 2 },
        Rectangle { WINDOW_WIDTH, BOARD_BORDER },
        style,
        Visible { },
        Wall { }
    );

    // Left
    world.createEntity(
        BounceCollision { },
        Position { BOARD_BORDER / 2, WINDOW_HEIGHT / 2 },
        Rectangle { BOARD_BORDER, WINDOW_HEIGHT },
        style,
        Visible { },
        Wall { }
    );

    // Right
    world.createEntity(
        BounceCollision { },
        Position { WINDOW_WIDTH - BOARD_BORDER / 2, WINDOW_HEIGHT / 2 },
        Rectangle { BOARD_BORDER, WINDOW_HEIGHT },
        style,
        Visible { },
        Wall { }
    );

    // Bottom
    world.createEntity(
        BounceCollision { },
        Position { WINDOW_WIDTH / 2, 0 },
        Rectangle { WINDOW_WIDTH, BOARD_BORDER },
        style,
        Visible { },
        Wall { }
    );
}
