#pragma once

#include "DataQuery.hpp"
#include "ECS.hpp"

namespace ecs {
    /**
     * Manages the game entities and their components.
     *
     * In the Entity-Component-System architecture, entities are simple GUIDs.
     * Components by themselves have no logic and can be bound to entities to
     * add data to them. All the game logic is implemented by systems, which
     * operate on the data bound to the entities.
     */
    template<typename ECS>
    class GenericComponentManager {
     public:
        /**
         * Creates a new entity with the given components, if any.
         * Returns the ID of the created entity.
         */
        template<typename... Ts>
        Entity createEntity(Ts&&...);

        /**
         * Adds a given T component to an entity.
         */
        template<typename T>
        void addComponent(Entity, T&&);

        /**
         * Removes the T component from an entity.
         */
        template<typename T>
        void removeComponent(Entity);

        /**
         * Checks if an entity has a T component.
         */
        template<typename T>
        bool hasComponent(Entity) const;

        /**
         * Checks if an entity has all the input components.
         */
        template<typename... Ts>
        bool hasAllComponents(Entity) const;

        /**
         * Returns the T component data of an entity. Throws if
         * the entity doesn't have the T component.
         */
        template<typename T>
        T& getData(Entity);

        /**
         * Iterates over all entities that have all the input components,
         * executing a callback for each of them.
         *
         * The entity itself and its data for all the input components is
         * passed to the callback. `fn` must be compatible with the following
         * signature:
         *
         * `std::function<void(Entity, T&, std::add_lvalue_reference_t<Ts>...)> fn`
         *
         * **Note**: the performance of this method increases if the least common
         * components come first in the list, especially the first.
         *
         * **Warning**: `fn` **must not** change the iterated entities, e.g it
         * must not attach/detach components that are used as input. If that
         * behavior is desired, use `mutatingQuery` instead.
         */
        template<typename T, typename... Ts, typename Functor>
        void query(Functor);

        /**
         * Functionally equal to `query`, but allows the input function to
         * mutate the components of the iterated entities. This is achieved
         * through an extra copy, and may have performance implications if
         * many entities have the T component.
         */
        template<typename T, typename... Ts, typename Functor>
        void mutatingQuery(Functor);

        /**
         * Notifies all entities with a listener-like T component, forwarding
         * the given arguments, if any, to them.
         */
        template<typename T, typename... Args>
        void notify(Args&&...);

        /**
         * Allows more "functional-style" queries. Returns a `DataQuery` that
         * filters all entities with a T component.
         */
        template<typename T>
        DataQuery<ECS, T> findAll();

     private:
        ECS storage;

        template<typename T, typename... Ts, typename Functor>
        void internalQuery(Functor, ComponentData<T>&);
    };


    template<typename ECS>
    template<typename... Ts>
    inline Entity GenericComponentManager<ECS>::createEntity(Ts&&... data) {
        Entity id = storage.nextEntityId++;
        (addComponent<Ts>(id, std::forward<Ts>(data)), ...);
        return id;
    }

    template<typename ECS>
    template<typename T>
    inline void GenericComponentManager<ECS>::addComponent(Entity entity, T&& data) {
        entityData<std::decay_t<T>>(storage).insert({
            entity,
            std::forward<T>(data)
        });
    }

    template<typename ECS>
    template<typename T>
    inline void GenericComponentManager<ECS>::removeComponent(Entity entity) {
        entityData<T>(storage).erase(entity);
    }

    template<typename ECS>
    template<typename T>
    inline bool GenericComponentManager<ECS>::hasComponent(Entity entity) const {
        return entityData<T>(storage).count(entity);
    }

    template<typename ECS>
    template<typename... Ts>
    bool GenericComponentManager<ECS>::hasAllComponents(Entity entity) const {
        return (hasComponent<Ts>(entity) && ...);
    }

    template<typename ECS>
    template<typename T>
    inline T& GenericComponentManager<ECS>::getData(Entity entity) {
        return entityData<T>(storage).at(entity);
    }

    template<typename ECS>
    template<typename T, typename... Ts, typename Functor>
    inline void GenericComponentManager<ECS>::query(Functor fn) {
        ComponentData<T>& baseData = entityData<T>(storage);
        internalQuery<T, Ts...>(fn, baseData);
    }

    template<typename ECS>
    template<typename T, typename... Ts, typename Functor>
    inline void GenericComponentManager<ECS>::mutatingQuery(Functor fn) {
        // Makes a copy due to potential iterator invalidation
        ComponentData<T> baseData = entityData<T>(storage);
        internalQuery<T, Ts...>(fn, baseData);
    }

    template<typename ECS>
    template<typename T, typename... Args>
    inline void GenericComponentManager<ECS>::notify(Args&&... args) {
        mutatingQuery<T>([&](Entity, T& listener) {
            listener.fn(std::forward<Args>(args)...);
        });
    }

    template<typename ECS>
    template<typename T>
    inline DataQuery<ECS, T> GenericComponentManager<ECS>::findAll() {
        return DataQuery<ECS, T>(storage);
    }

    template<typename ECS>
    template<typename T, typename... Ts, typename Functor>
    inline void GenericComponentManager<ECS>::internalQuery(Functor fn, ComponentData<T>& baseData) {
        if constexpr (sizeof...(Ts) > 0) {
            for (auto& [entity, data] : baseData) {
                if (hasAllComponents<Ts...>(entity)) {
                    fn(entity, data, getData<Ts>(entity)...);
                }
            }
        } else {
            for (auto& [entity, data] : baseData) {
                fn(entity, data);
            }
        }
    }
}
