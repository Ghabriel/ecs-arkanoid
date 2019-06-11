#pragma once

#include "../components/Circle.hpp"
#include "../components/Position.hpp"
#include "../components/Rectangle.hpp"
#include "../components/Visible.hpp"
#include "../engine/ecs/include.hpp"

namespace ecs {
    using ECS = GenericECS<
        Circle,
        Position,
        Rectangle,
        Visible
    >;

    using ComponentManager = GenericComponentManager<ECS>;
}
