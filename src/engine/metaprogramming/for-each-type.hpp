#pragma once

#include <tuple>

namespace meta {
    namespace __detail {
        template<typename F, typename... Ts>
        void forEachT(F fn, std::tuple<Ts...>*) {
            (fn.template operator()<Ts>(), ...);
        }
    }

    template<typename Tuple, typename F>
    void forEachT(F fn) {
        return __detail::forEachT(fn, static_cast<Tuple*>(nullptr));
    }
}
