#pragma once

#include "../../engine-glue/ecs.hpp"

void useBallPaddleCollisionSystem(ecs::World&, ecs::Entity, ecs::Entity);
void useBounceCollisionSystem(ecs::World&, ecs::Entity, const metadata::MultiCollisionData&);
void usePaddleWallCollisionSystem(ecs::World&, ecs::Entity, ecs::Entity);
