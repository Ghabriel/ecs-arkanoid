#pragma once

#include "../components/Position.hpp"
#include "../engine/ecs/include.hpp"

namespace ecs {
    using ECS = GenericECS<
        Position
    >;

    using ComponentManager = GenericComponentManager<ECS>;
}
