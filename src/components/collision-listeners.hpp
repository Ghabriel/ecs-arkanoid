#pragma once

#include <functional>
#include <vector>
#include "../engine/ecs/ECS.hpp"

namespace metadata {
    struct CollisionData {
        ecs::Entity objectId;
        bool collidesInX;
        bool collidesInY;
    };

    using MultiCollisionData = std::vector<metadata::CollisionData>;
}

struct BallPaddleCollisionListener {
    std::function<void(ecs::Entity, ecs::Entity)> fn;
};

struct BallObjectsCollisionListener {
    std::function<void(ecs::Entity, const metadata::MultiCollisionData&)> fn;
};

struct PaddleWallCollisionListener {
    std::function<void(ecs::Entity, ecs::Entity)> fn;
};
