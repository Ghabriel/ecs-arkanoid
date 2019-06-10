#pragma once

#include <functional>

namespace ecs {
    namespace __detail {
        // Converts a functor into an appropriate function pointer.
        template<typename F>
        struct helper_lambda_to_pointer {
            using type = decltype(&F::operator());
        };


        // Converts a function pointer into an appropriate std::function,
        // or void for other types.
        template<typename T>
        struct helper_pointer_to_function {
            using type = void;
        };

        template<typename Ret, typename Class, typename... Args>
        struct helper_pointer_to_function<Ret(Class::*)(Args...) const> {
            using type = std::function<Ret(Args...)>;
        };


        // Converts an std::function into a tuple of its argument types.
        template<typename F>
        struct helper_tuple_argument_types;

        template<typename Ret, typename... Args>
        struct helper_tuple_argument_types<std::function<Ret(Args...)>> {
            using type = std::tuple<Args...>;
        };


        // Applies the appropriate type conversions to get a tuple of argument
        // types from a lambda, if possible (ill-formed if it's not).
        template<typename F>
        struct helper_type_conversions {
            using FunctionPointer = typename helper_lambda_to_pointer<F>::type;
            using FunctionInstance = typename helper_pointer_to_function<FunctionPointer>::type;
            using ArgTuple = typename helper_tuple_argument_types<FunctionInstance>::type;
        };
    }

    template<typename F>
    struct lambda_argument_types {
        using type = typename __detail::helper_type_conversions<F>::ArgTuple;
    };

    template<typename F>
    using lambda_argument_types_t = typename lambda_argument_types<F>::type;
}
