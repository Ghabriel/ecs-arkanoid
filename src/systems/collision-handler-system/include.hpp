#pragma once

#include "../../engine-glue/ecs.hpp"

void useCollisionHandlerSystem(ecs::World&, ecs::Entity, ecs::Entity);
void useCollisionHandlerSystem(ecs::World&, ecs::Entity, const metadata::MultiCollisionData&);
