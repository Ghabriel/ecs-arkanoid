#pragma once

#include "../../engine-glue/ecs.hpp"

template<typename T, typename U>
void useCollisionSystem(ecs::World&, ecs::Entity, metadata::CollisionData<T, U>);
