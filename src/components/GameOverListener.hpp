#pragma once

#include <functional>

struct GameOverListener {
    std::function<void()> fn;
};
