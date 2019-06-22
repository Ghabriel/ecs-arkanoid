#include "include.hpp"

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
