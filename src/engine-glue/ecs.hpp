#pragma once

#include "../components/collision-listeners.hpp"
#include "../components/Circle.hpp"
#include "../components/GameOverListener.hpp"
#include "../components/Link.hpp"
#include "../components/Position.hpp"
#include "../components/powerups.hpp"
#include "../components/Rectangle.hpp"
#include "../components/Style.hpp"
#include "../components/Tags.hpp"
#include "../components/Velocity.hpp"
#include "../engine/ecs/include.hpp"

namespace ecs {
    using ECS = GenericECS<
        Ball,
        BounceCollision,
        Brick,
        Circle,
        CollisionListener<Ball, Brick>,
        CollisionListener<Ball, Paddle>,
        CollisionListener<Ball, Wall>,
        CollisionListener<Paddle, PowerUp>,
        CollisionListener<Paddle, Wall>,
        GameOverListener,
        Input,
        Link,
        Paddle,
        PiercingBall,
        Position,
        PowerUp,
        Rectangle,
        Style,
        Velocity,
        Visible,
        Wall
    >;

    using World = GenericWorld<ECS>;

    template<typename T, typename... Ts>
    using DataQuery = GenericDataQuery<ECS, T, Ts...>;
}
