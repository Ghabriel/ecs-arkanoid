#pragma once

#include <functional>
#include <vector>
#include "../engine/ecs/ECS.hpp"
#include "Tags.hpp"

namespace metadata {
    struct CollisionData {
        ecs::Entity objectId;
        bool collidesInX;
        bool collidesInY;
    };

    using MultiCollisionData = std::vector<metadata::CollisionData>;
}

template<typename T, typename U>
struct CollisionListener {
    std::function<void(ecs::Entity, std::vector<ecs::Entity>)> fn;
};

template<>
struct CollisionListener<Ball, Brick> {
    std::function<void(ecs::Entity, const metadata::MultiCollisionData&)> fn;
};

template<>
struct CollisionListener<Ball, Wall> {
    std::function<void(ecs::Entity, const metadata::MultiCollisionData&)> fn;
};
