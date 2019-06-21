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
        BallPaddleCollisionListener,
        BallObjectsCollisionListener,
        BounceCollision,
        Brick,
        Circle,
        GameOverListener,
        Input,
        Link,
        Paddle,
        PaddlePowerUpCollisionListener,
        PaddleWallCollisionListener,
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
