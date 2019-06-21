#pragma once

#include <random>

namespace misc {
    inline bool checkPercentage(int percentage) {
        static std::random_device randomDevice;
        static std::mt19937 gen(randomDevice());
        static std::uniform_int_distribution<> distribution(1, 100);

        return distribution(gen) <= percentage;
    }
}
