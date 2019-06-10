#pragma once

#include <unordered_map>

namespace ecs {
    using Entity = unsigned;

    template<typename T>
    using ComponentData = std::unordered_map<Entity, T>;

    namespace __detail {
        template<typename T>
        struct FieldContainer {
            ComponentData<T> field;
            static constexpr ComponentData<T> FieldContainer::* address = &FieldContainer::field;
        };
    }

    template<typename... Ts>
    struct GenericECS : __detail::FieldContainer<Ts>... {
        Entity nextEntityId = 0;
    };

    template<typename T, typename ECS>
    constexpr ComponentData<T>& entityData(ECS& ecs) {
        constexpr auto field = __detail::FieldContainer<T>::address;
        return ecs.*field;
    }

    template<typename T, typename ECS>
    constexpr const ComponentData<T>& entityData(const ECS& ecs) {
        constexpr auto field = __detail::FieldContainer<T>::address;
        return ecs.*field;
    }
}
