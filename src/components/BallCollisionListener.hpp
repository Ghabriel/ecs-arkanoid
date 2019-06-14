#pragma once

#include <functional>
#include "../engine/ecs/ECS.hpp"

struct BallCollisionListener {
    std::function<void(ecs::Entity)> fn;
};
