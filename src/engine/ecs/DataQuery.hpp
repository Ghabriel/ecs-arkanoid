#pragma once

#include <tuple>
#include <type_traits>
#include "../metaprogramming/lambda-argument-types.hpp"
#include "ECS.hpp"

namespace ecs {
    namespace __detail {
        template<typename T>
        struct QueryParameter {
            template<typename ECS>
            static T& get(ECS& storage, Entity entity) {
                return entityData<std::decay_t<T>>(storage).at(entity);
            }
        };

        template<>
        struct QueryParameter<Entity> {
            template<typename ECS>
            static Entity get(ECS& storage, Entity entity) {
                return entity;
            }
        };

        template<typename T>
        struct Dispatcher;

        template<typename... Ts>
        struct Dispatcher<std::tuple<Ts...>> {
            template<typename ECS, typename Functor>
            void operator()(ECS& storage, Functor fn, Entity entity) {
                fn(QueryParameter<Ts>::get(storage, entity)...);
            }
        };
    }

    template<typename... Ts>
    struct Desirable { };

    template<typename... Ts>
    struct Undesirable { };

    template<typename ECS, typename... Ts>
    class GenericDataQuery;

    template<typename ECS, typename T, typename... Ts, typename... Us>
    class GenericDataQuery<ECS, Desirable<T, Ts...>, Undesirable<Us...>> {
    public:
        explicit GenericDataQuery(ECS& storage) : storage(storage) { }

        /**
         * Returns a `GenericDataQuery` with an additional `U` filter.
         */
        template<typename U>
        auto join() const {
            return GenericDataQuery<
                ECS,
                Desirable<T, Ts..., U>,
                Undesirable<Us...>
            >(storage);
        }

        /**
         * Returns a `GenericDataQuery` that ignores entities with `U` components.
         */
        template<typename U>
        auto ignore() const {
            return GenericDataQuery<
                ECS,
                Desirable<T, Ts...>,
                Undesirable<Us..., U>
            >(storage);
        }

        /**
         * Iterates over all entities that match this `GenericDataQuery` filters,
         * executing a callback for each of them.
         *
         * The callback itself dictates which data is passed to it. Its
         * parameters can be either T, T& or const T& for any combination of
         * filtered Ts.
         *
         * **Warning**: `fn` **must not** change the iterated entities, e.g it
         * must not attach/detach components that are used as input. If that
         * behavior is desired, use `mutatingForEach` instead.
         */
        template<typename Functor>
        void forEach(Functor fn) {
            ComponentData<T>& baseData = entityData<T>(storage);
            internalForEach(fn, baseData);
        }

        /**
         * Functionally equal to `forEach`, but allows the input function to
         * mutate the components of the iterated entities. This is achieved
         * through an extra copy, and may have performance implications if
         * many entities have the first filtered component.
         */
        template<typename Functor>
        void mutatingForEach(Functor fn) {
            // Makes a copy due to potential iterator invalidation
            ComponentData<T> baseData = entityData<T>(storage);
            internalForEach(fn, baseData);
        }

    private:
        ECS& storage;

        template<typename U>
        bool hasComponent(Entity entity) const {
            return entityData<U>(storage).count(entity);
        }

        template<typename... Vs>
        bool hasAllComponents(Entity entity) const {
            return (hasComponent<Vs>(entity) && ...);
        }

        template<typename... Vs>
        bool hasNoComponents(Entity entity) const {
            return (!hasComponent<Vs>(entity) && ...);
        }

        template<typename Functor>
        void internalForEach(Functor fn, ComponentData<T>& baseData) {
            __detail::Dispatcher<meta::lambda_argument_types_t<Functor>> dispatcher;

            if constexpr (sizeof...(Ts) > 0 || sizeof...(Us) > 0) {
                for (auto& [entity, data] : baseData) {
                    if (hasAllComponents<Ts...>(entity) && hasNoComponents<Us...>(entity)) {
                        dispatcher(storage, fn, entity);
                    }
                }
            } else {
                for (auto& [entity, data] : baseData) {
                    dispatcher(storage, fn, entity);
                }
            }
        }
    };
}
