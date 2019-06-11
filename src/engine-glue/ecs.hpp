#pragma once

#include "../components/Circle.hpp"
#include "../components/Position.hpp"
#include "../components/Rectangle.hpp"
#include "../engine/ecs/include.hpp"

namespace ecs {
    using ECS = GenericECS<
        Circle,
        Position,
        Rectangle
    >;

    using ComponentManager = GenericComponentManager<ECS>;
}
