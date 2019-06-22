#include "include.hpp"

#include <bitset>
#include <cassert>
#include "../../constants.hpp"
#include "../../engine/misc/check-percentage.hpp"
#include "../../helpers/aggregate-data.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

#include <iostream>

template<typename F>
static void handleBounceCollisions(
    ecs::World&,
    ecs::Entity,
    const metadata::MultiRectCollisionData&,
    F
);
static bool handleBallBrickCollision(ecs::World&, ecs::Entity, ecs::Entity);
static bool handleBallWallCollision(ecs::World&, ecs::Entity, ecs::Entity);

template<>
void useCollisionSystem<Ball, Paddle>(
    ecs::World& world,
    ecs::Entity ballId,
    metadata::CollisionData<Ball, Paddle> paddleIds
) {
    for (ecs::Entity paddleId : paddleIds) {
        std::cout << "Collision detected with Paddle\n";
        const Position& ballPos = world.getData<Position>(ballId);
        const Position& paddlePos = world.getData<Position>(paddleId);
        world.getData<Velocity>(ballId) = getBallNewVelocity(ballPos, paddlePos);
    }
}

template<>
void useCollisionSystem<Ball, Brick>(
    ecs::World& world,
    ecs::Entity ballId,
    metadata::CollisionData<Ball, Brick> collisions
) {
    handleBounceCollisions(
        world,
        ballId,
        collisions,
        [](ecs::World& world, ecs::Entity ballId, ecs::Entity brickId) {
            return handleBallBrickCollision(world, ballId, brickId);
        }
    );
}

template<>
void useCollisionSystem<Ball, Wall>(
    ecs::World& world,
    ecs::Entity ballId,
    metadata::CollisionData<Ball, Wall> collisions
) {
    handleBounceCollisions(
        world,
        ballId,
        collisions,
        [](ecs::World& world, ecs::Entity ballId, ecs::Entity wallId) {
            return handleBallWallCollision(world, ballId, wallId);
        }
    );
}

template<>
void useCollisionSystem<Paddle, PowerUp>(
    ecs::World& world,
    ecs::Entity paddleId,
    metadata::CollisionData<Paddle, PowerUp> powerUpIds
) {
    for (ecs::Entity powerUpId : powerUpIds) {
        std::cout << "Collision detected between Paddle and PowerUp " << powerUpId << "\n";
        world.deleteEntity(powerUpId);

        world.findAll<Ball>()
            .forEach([&world](ecs::Entity ballId) {
                if (world.hasComponent<PiercingBall>(ballId)) {
                    return;
                }

                std::cout << "Ball is now in piercing mode.\n";

                auto expirationFn = [&world, ballId] {
                    std::cout << "Piercing mode expired.\n";
                    world.removeComponent<PiercingBall>(ballId);
                };

                world.createEntity(createEvent(3000, expirationFn));

                world.addComponent(ballId, PiercingBall { });
            });
    }
}

template<>
void useCollisionSystem<Paddle, Wall>(
    ecs::World& world,
    ecs::Entity paddleId,
    metadata::CollisionData<Paddle, Wall> wallIds
) {
    assert(wallIds.size() == 1);

    ecs::Entity wallId = wallIds[0];
    std::cout << "Collision detected between Paddle and Wall " << wallId << "\n";

    Rectangle& paddleBody = world.getData<Rectangle>(paddleId);
    Position& paddlePos = world.getData<Position>(paddleId);
    Velocity& paddleVelocity = world.getData<Velocity>(paddleId);
    RectangleData paddle { paddleBody, paddlePos };

    const Rectangle& wallBody = world.getData<Rectangle>(wallId);
    const Position& wallPos = world.getData<Position>(wallId);
    RectangleData wall { wallBody, wallPos };

    std::array<float, 4> ts {
        (wall.rightX() - paddle.leftX()) / paddleVelocity.x,
        (wall.leftX() - paddle.rightX()) / paddleVelocity.x,
        (wall.bottomY() - paddle.topY()) / paddleVelocity.y,
        (wall.topY() - paddle.bottomY()) / paddleVelocity.y
    };

    float minValidT = 1;

    for (float t : ts) {
        if (t >= 0 && t < minValidT) {
            minValidT = t;
        }
    }

    paddlePos += paddleVelocity * minValidT;
    world.removeComponent<Velocity>(paddleId);
}

#define INSTANTIATE(T, U) \
template void useCollisionSystem<T, U>(\
    ecs::World&,\
    ecs::Entity,\
    metadata::CollisionData<T, U>\
);

INSTANTIATE(Ball, Paddle);
INSTANTIATE(Ball, Brick);
INSTANTIATE(Ball, Wall);
INSTANTIATE(Paddle, PowerUp);
INSTANTIATE(Paddle, Wall);
#undef INSTANTIATE

// ----------------------------------------------------
// Helper functions
// ----------------------------------------------------

template<typename F>
void handleBounceCollisions(
    ecs::World& world,
    ecs::Entity ballId,
    const metadata::MultiRectCollisionData& collisions,
    F shouldIgnoreCollisionFn
) {
    bool collidesInX = false;
    bool collidesInY = false;

    for (const metadata::RectCollisionData& collisionData : collisions) {
        if (!shouldIgnoreCollisionFn(world, ballId, collisionData.objectId)) {
            collidesInX = collidesInX || collisionData.collidesInX;
            collidesInY = collidesInY || collisionData.collidesInY;
        }
    }

    Velocity& ballVelocity = world.getData<Velocity>(ballId);

    if (collidesInX) {
        ballVelocity.x *= -1;
    }

    if (collidesInY) {
        ballVelocity.y *= -1;
    }
}

bool handleBallBrickCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity brickId
) {
    std::cout << "Collision detected with brick " << brickId << '\n';

    if (world.hasComponent<PiercingBall>(ballId)) {
        world.deleteEntity(brickId);
        return true;
    }

    if (misc::checkPercentage(50)) {
        using constants::POWERUP_RADIUS;
        using constants::POWERUP_VELOCITY;

        const Position& brickPos = world.getData<Position>(brickId);

        world.createEntity(
            Circle { POWERUP_RADIUS },
            brickPos,
            PowerUp { },
            Style { sf::Color::Red, sf::Color::Blue, 2 },
            Velocity { 0, POWERUP_VELOCITY },
            Visible { }
        );
    }

    world.deleteEntity(brickId);
    return false;
}

bool handleBallWallCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity wallId
) {
    std::cout << "Collision detected with wall " << wallId << '\n';
    return false;
}
