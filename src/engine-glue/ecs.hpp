#pragma once

#include "../components/BallCollisionListener.hpp"
#include "../components/Circle.hpp"
#include "../components/Link.hpp"
#include "../components/Position.hpp"
#include "../components/Rectangle.hpp"
#include "../components/Tags.hpp"
#include "../components/Velocity.hpp"
#include "../engine/ecs/include.hpp"

namespace ecs {
    using ECS = GenericECS<
        Ball,
        BallCollisionListener,
        BounceCollision,
        Brick,
        Circle,
        Input,
        Link,
        Paddle,
        Position,
        Rectangle,
        Velocity,
        Visible,
        Wall
    >;

    using World = GenericWorld<ECS>;

    template<typename T, typename... Ts>
    using DataQuery = GenericDataQuery<ECS, T, Ts...>;
}
