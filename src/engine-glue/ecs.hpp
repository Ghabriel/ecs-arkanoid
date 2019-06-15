#pragma once

#include "../components/Ball.hpp"
#include "../components/BallCollisionListener.hpp"
#include "../components/BounceCollision.hpp"
#include "../components/Brick.hpp"
#include "../components/Circle.hpp"
#include "../components/Input.hpp"
#include "../components/Paddle.hpp"
#include "../components/Position.hpp"
#include "../components/Rectangle.hpp"
#include "../components/Velocity.hpp"
#include "../components/Visible.hpp"
#include "../engine/ecs/include.hpp"

namespace ecs {
    using ECS = GenericECS<
        Ball,
        BallCollisionListener,
        BounceCollision,
        Brick,
        Circle,
        Input,
        Paddle,
        Position,
        Rectangle,
        Velocity,
        Visible
    >;

    using ComponentManager = GenericComponentManager<ECS>;

    template<typename T, typename... Ts>
    using DataQuery = GenericDataQuery<ECS, T, Ts...>;
}
