#pragma once

#include <tuple>

namespace meta {
    template<typename... Ts>
    struct ForEachT;

    template<typename... Ts>
    struct ForEachT<std::tuple<Ts...>> {
        template<typename F>
        static void exec(F fn) {
            (fn.template operator()<Ts>(), ...);
        }
    };
}
