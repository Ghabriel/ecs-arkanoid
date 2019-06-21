#include "include.hpp"

#include <bitset>
#include <cassert>
#include "../../constants.hpp"
#include "../../engine/misc/check-percentage.hpp"
#include "../../helpers/aggregate-data.hpp"
#include "../../helpers/ball-paddle-contact.hpp"

#include <iostream>

static bool handleBounceCollision(ecs::World&, ecs::Entity, ecs::Entity);
static bool handleBallBrickCollision(ecs::World&, ecs::Entity, ecs::Entity);

void useBallPaddleCollisionSystem(
    ecs::World& world,
    ecs::Entity ballId,
    const std::vector<ecs::Entity>& paddleIds
) {
    for (ecs::Entity paddleId : paddleIds) {
        std::cout << "Collision detected with Paddle\n";
        const Position& ballPos = world.getData<Position>(ballId);
        const Position& paddlePos = world.getData<Position>(paddleId);
        world.getData<Velocity>(ballId) = getBallNewVelocity(ballPos, paddlePos);
    }
}

void useBounceCollisionSystem(
    ecs::World& world,
    ecs::Entity ballId,
    const metadata::MultiCollisionData& collisions
) {
    bool collidesInX = false;
    bool collidesInY = false;

    for (const metadata::CollisionData& collisionData : collisions) {
        bool ignoreCollision = handleBounceCollision(world, ballId, collisionData.objectId);

        if (!ignoreCollision) {
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

void usePaddlePowerUpCollisionSystem(
    ecs::World& world,
    ecs::Entity paddleId,
    const std::vector<ecs::Entity>& powerUpIds
) {
    for (ecs::Entity powerUpId : powerUpIds) {
        std::cout << "Collision detected between Paddle and PowerUp " << powerUpId << "\n";

        world.findAll<Ball>()
            .forEach([&world](ecs::Entity ballId) {
                std::cout << "Ball is now in piercing mode.\n";
                world.addComponent(ballId, PiercingBall { });
            });
    }
}

void usePaddleWallCollisionSystem(
    ecs::World& world,
    ecs::Entity paddleId,
    const std::vector<ecs::Entity>& wallIds
) {
    assert(wallIds.size() <= 1);

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

bool handleBounceCollision(
    ecs::World& world,
    ecs::Entity ballId,
    ecs::Entity objectId
) {
    if (world.hasComponent<Brick>(objectId)) {
        return handleBallBrickCollision(world, ballId, objectId);
    } else {
        std::cout << "Collision detected with wall " << objectId << '\n';
        return false;
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
