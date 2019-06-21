#pragma once

#include "../../engine-glue/ecs.hpp"

void useBallPaddleCollisionSystem(ecs::World&, ecs::Entity, const std::vector<ecs::Entity>&);
void useBallBrickCollisionSystem(ecs::World&, ecs::Entity, const metadata::MultiCollisionData&);
void useBallWallCollisionSystem(ecs::World&, ecs::Entity, const metadata::MultiCollisionData&);
void usePaddlePowerUpCollisionSystem(ecs::World&, ecs::Entity, const std::vector<ecs::Entity>&);
void usePaddleWallCollisionSystem(ecs::World&, ecs::Entity, const std::vector<ecs::Entity>&);
