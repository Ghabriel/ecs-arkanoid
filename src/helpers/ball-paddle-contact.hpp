#pragma once

#include <cmath>
#include "../components/Position.hpp"
#include "../components/Velocity.hpp"
#include "../constants.hpp"

inline Velocity getBallNewVelocity(const Position& ballPos, const Position& paddlePos) {
    float dx = ballPos.x - paddlePos.x;
    float dy = ballPos.y - paddlePos.y;
    float angle = std::atan2(dy, dx);

    using constants::BALL_VELOCITY;

    return Velocity {
        BALL_VELOCITY * std::cos(angle),
        BALL_VELOCITY * std::sin(angle)
    };
}
