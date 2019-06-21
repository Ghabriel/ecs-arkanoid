#pragma once

#include <functional>
#include <vector>
#include "../engine/ecs/ECS.hpp"
#include "Tags.hpp"

namespace metadata {
    struct RectCollisionData {
        ecs::Entity objectId;
        bool collidesInX;
        bool collidesInY;
    };

    using MultiRectCollisionData = const std::vector<RectCollisionData>&;

    template<typename T, typename U>
    struct CollisionDataWrapper {
        using Type = const std::vector<ecs::Entity>&;
    };

    template<>
    struct CollisionDataWrapper<Ball, Brick> {
        using Type = MultiRectCollisionData;
    };

    template<>
    struct CollisionDataWrapper<Ball, Wall> {
        using Type = MultiRectCollisionData;
    };

    template<typename T, typename U>
    using CollisionData = typename CollisionDataWrapper<T, U>::Type;
}

template<typename T, typename U>
struct CollisionListener {
    std::function<void(ecs::Entity, metadata::CollisionData<T, U>)> fn;
};
