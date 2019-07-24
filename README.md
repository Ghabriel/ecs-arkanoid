# Overview - ECS Framework

This document aims to explain the core ideas behind the ECS Framework used, providing examples originated from the [ecs-pong](https://github.com/Ghabriel/ecs-pong) and [ecs-arkanoid](https://github.com/Ghabriel/ecs-arkanoid) projects.

## Game class

The project has a `Game` class, which acts as a facade to the underlying game logic. It contains two private properties:
 * `ecs::World world` - a data structure containing all game entities, including their components (more details below);
* `StateMachine stateMachine` - a generic state machine that controls the state transitions within the game.

The following three methods are provided and recommended for any game:
 * `void init()` - registers the game state instances to the state machine, and pushes the initial state. Additional arguments may be supplied. For example, ecs-arkanoid receives the size of the window and implements `init` as follows:
    ```cpp
    void init(float width, float height) {
        stateMachine.registerState("waiting", std::make_unique<WaitingState>(world, stateMachine));
        stateMachine.registerState("running", std::make_unique<RunningState>(world, stateMachine));
        stateMachine.pushState("waiting");
    }
    ```
 * `void update(const sf::Time& elapsedTime)` - updates the game state. This is usually just a function call to:
    ```cpp
    stateMachine.getState().update(elapsedTime);
    ```
    The parameter indicates the time passed since the last call to `update`.
 * `void render(sf::RenderWindow& window)` - renders the current game state. This is usually just a function call to:
    ```cpp
    stateMachine.getState().render(window);
    ```
    The parameter contains the SFML instance of the game window.

## States

The framework is based on **states**. A `State` is a simple interface with the following structure:
```cpp
namespace state {
    class State {
     public:
        virtual ~State() = default;

        virtual void onEnter() { }
        virtual void onExit() { }
        virtual void update(const sf::Time& elapsedTime) = 0;
        virtual void render(sf::RenderWindow& window) = 0;
    };
}
```

Classes can extend from this interface to provide arbitrary game logic. They usually contain, at least, the same internal structures of `Game`. The following code illustrates `WaitingState`, a very simple state used in ecs-pong:
```cpp
class WaitingState : public state::State {
 public:
    WaitingState(
        ecs::World& world,
        state::StateMachine& stateMachine
    ) : world(world), stateMachine(stateMachine) { }

    void update(const sf::Time& elapsedTime) override {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            stateMachine.pushState("running");
        }
    }

    void render(sf::RenderWindow& window) override {
        useRenderingSystem(world, window);
    }

 private:
    ecs::World& world;
    state::StateMachine& stateMachine;
};
```

Note that the constructor can safely take `World` and `StateMachine` by reference, since `Game` is always in scope and owns them.

`WaitingState` simply waits for the player to press the spacebar and, when they do, changes the current state to `"running"`. The state machine maps names to state instances, decoupling the state classes. It's up to `Game`, inside `init()`, to dictate which names map to which instances.

The `render` method of `WaitingState` calls a **system**, a core idea of the ECS architecture. Systems are arbitrary functions, conventionally prefixed by `use`, which take the `world` as parameter (and possibly other things). A system is usually a free function with its own files (inside `src/systems/` in the conventional file structure, explained below), facilitating its reuse by multiple states.

### onEnter and onExit

The `State` interface has two methods that are called whenever a state transition happens, named `onEnter` and `onExit`. They are usually used to remove entities (or detach components) in the exiting state and to add new entities (or attach components) in the entering state.

It's very common for operations in `onEnter` to be "undone" in `onExit`. This typically leads to the rather fragile code structure below:
```cpp
void onEnter() override {
    operationA();
    operationB();
    operationC();
}

void onExit() override {
    undoOperationA();
    undoOperationB();
    undoOperationC();
}
```

Since each pair of related operations (`operationA` / `undoOperationA`, `operationB` / `undoOperationB`, etc) is distributed across two methods, it's very easy to make a change in one method and forget to reflect it in the other. Inspired by [React](https://github.com/facebook/react), the framework solves this issue by introducing the idea of "effects".

An effect is an operation that has an associated cleanup function. The previous example becomes the following when expressed with effects:
```cpp
void onEnter() override {
    useEffect([] {
        operationA();
        return undoOperationA;
    });

    useEffect([] {
        operationB();
        return undoOperationB;
    });

    useEffect([] {
        operationC();
        return undoOperationC;
    });
}
```
Notice how each pair of related operations is now grouped together, simplifying the maintenance of the code and promoting code reuse.

Effects can be used in the framework by inheriting from `state::EffectState` instead of `state::State`. It provides two additional methods over `State`:

 * `useEffect`:
    ```cpp
    template<typename F>
    void useEffect(F fn);
    ```
    Described above. `fn` must be compatible with the following signature:
    ```cpp
    std::function<void()> fn();
    ```

 * `useToggleComponentEffect`:
    ```cpp
    template<typename T, typename ECS>
    void useToggleComponentEffect(
        ecs::GenericWorld<ECS>& world,
        ecs::Entity entity,
        const T& component
    );
    ```
    Applies the following effect:
     * `onEnter`: adds the `T` component to the given entity;
     * `onExit`: removes the `T` component from the given entity.

    The following overload can be added to a concrete state class to make the `world` parameter no longer needed:
    ```cpp
    template<typename T>
    void useToggleComponentEffect(ecs::Entity entity, const T& component) {
        EffectState::useToggleComponentEffect(world, entity, component);
    }
    ```
    **Note:** this snippet assumes that your class stores the world in a variable called `world`; adjust the code as needed.

Additionally, it automatically implements `onExit` according to the effects used, making subclasses only responsible for `onEnter`, `update` and `render`.

## Entities and components

Alongside _systems_, other two core ideas of ECS are the **entities** and the **components**, illustrated below.

![ecs](https://i.stack.imgur.com/yuDjY.gif)

Entities are simple GUIDs; they are used as indexes in an _ECS Storage_ to allow retrieval of their components, as well as adding new ones or removing others. Components are structs without any methods, designed to hold data about a certain aspect of an entity. Consider the following example:
```cpp
struct Rectangle {
    float width;
    float height;
};
```

Entities that have a `Rectangle` component have a rectangular body with a certain `width` and `height`. This information can be retrieved and used by systems, e.g a collision detection system or a rendering system.

### World class

In the framework, there are three main generic classes that are responsible for ECS manipulation:
 * `template<typename... Ts> struct GenericECS` - stores all the game data related to entities and their components. The types `Ts` represent all the components that can be used;
 * `template<typename ECS> class GenericWorld` - a class that wraps an ECS Storage and provides methods for adding/removing entities, adding/removing/retrieving components from entities, etc;
 * `template<typename ECS, typename... Ts> class GenericDataQuery` - a class obtained from `GenericWorld` that provides a different, more functional-style way to manipulate an ECS Storage.

Since it would be extremely convoluted to refer to `GenericECS`, `GenericWorld` and `GenericDataQuery` everytime (with their large lists of type parameters), a file located in `src/engine-glue/ecs.hpp` is provided, in which sensible type aliases can be created for all of them. The following example is extracted from ecs-arkanoid:
```cpp
namespace ecs {
    using ECS = GenericECS<
        Ball,
        BounceCollision,
        Brick,
        Circle,
        /* omitted for brevity */
        Velocity,
        Visible,
        Wall
    >;

    using World = GenericWorld<ECS>;

    template<typename T, typename... Ts>
    using DataQuery = GenericDataQuery<ECS, T, Ts...>;
}
```

## Queries

The basic steps that almost every system follows are:
 1. Find all entities that will be needed for an operation. Filtering is done by querying entities by **which** components they have;

 2. Iterate over the entities, manipulating them by the filtered components.

Consider the following example, from the Movement System in ecs-arkanoid:
```cpp
void useMovementSystem(ecs::World& world, float elapsedTime) {
    world.findAll<Position>()
        .join<Velocity>()
        .forEach(
            [elapsedTime](Position& pos, const Velocity& v) {
                pos += v * elapsedTime;
            }
        );
}
```

First, it calls the method `world.findAll<Position>()`. This method returns a `DataQuery` that filters all entities that have a `Position` component.

Then, `.join<Velocity>()` is chained to it, which adds an extra filter to the `DataQuery`. At this point, we're filtering all entities that have **both** a `Position` and a `Velocity`.

Finally, `.forEach(...)` is added to the chain. This method finds all entities that pass our filters and iterates over them, executing a given function for each. The parameters of the function can be either:
 * `T`, `T&` or `const T&` for any combination of the filtered components;
 * `ecs::Entity`, which will contain the ID of the matched entity.

 All the following signatures (and many others) would compile correctly in our scenario:
```cpp
.forEach([](Position) { ... });
.forEach([](Position&, Velocity) { ... });
.forEach([](ecs::Entity, const Velocity&) { ... });
```

One important thing to note is that `.forEach()` **must not** change the list of iterated entities in any way. For example, if a component `T` is filtered, then you cannot do the following things inside the corresponding `.forEach()`:
 * Add the `T` component to an entity that didn't have it before;
 * Remove the `T` component from any entity.

If that kind of behavior is desired, you can use `.mutatingForEach()` instead, which works in the same way but allows these operations:
```cpp
void useTimingSystem(ecs::World& world) {
    auto now = std::chrono::system_clock::now();

    world.findAll<TimedEvent>()
        .mutatingForEach([&world, &now](ecs::Entity id, const TimedEvent& event) {
            if (now >= event.when) {
                event.fn();
                world.removeComponent<TimedEvent>(id);
            }
        });
}
```

**Tip:** both `.forEach()` and `.mutatingForEach()` perform better if the _least_ common component is the _first_ to be filtered.
