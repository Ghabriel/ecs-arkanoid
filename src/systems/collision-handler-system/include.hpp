#pragma once

#include "../../engine-glue/ecs.hpp"

void usePaddleCollisionHandlerSystem(ecs::World&, ecs::Entity, ecs::Entity);
void useBounceCollisionHandlerSystem(ecs::World&, ecs::Entity, const metadata::MultiCollisionData&);
