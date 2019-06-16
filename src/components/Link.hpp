#pragma once

#include "../engine/ecs/ECS.hpp"
#include "Position.hpp"

struct Link {
    ecs::Entity target;
    Position relativePosition;
};
