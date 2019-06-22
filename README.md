# Arkanoid - ECS

A simple implementation of Arkanoid using the ECS architectural pattern, C++17 and SFML.

## Components

| Component                          | Description |
|------------------------------------|-------------|
| Ball                               | tag component: entity is a ball |
| Brick                              | tag component: entity is a brick |
| Circle                             | radius of round objects |
| CollisionListener<Ball, Brick>     | contains a function that is called whenever a ball and a brick collide |
| CollisionListener<Ball, Paddle>    | contains a function that is called whenever a ball and a paddle collide |
| CollisionListener<Ball, Wall>      | contains a function that is called whenever a ball and a wall collide |
| CollisionListener<Paddle, PowerUp> | contains a function that is called whenever a paddle and a powerup collide |
| CollisionListener<Paddle, Wall>    | contains a function that is called whenever a paddle and a wall collide |
| GameOverListener                   | contains a function that is called whenever the player loses |
| Input                              | tag component: entity reacts to input |
| Link                               | links the position of an entity to another entity |
| Paddle                             | tag component: entity is a paddle |
| PiercingBall                       | tag component: ball has the Piercing Ball powerup |
| Position                           | location of the center of mass of the entity |
| PowerUp                            | tag component: entity is a powerup |
| Rectangle                          | width and height of rectangular objects |
| Style                              | fill color and border color/thickness |
| TimedEvent                         | contains a function that is called at a specific timestamp |
| Velocity                           | velocity of the entity |
| Visible                            | tag component: entity should be rendered |
| Wall                               | tag component: entity is a wall |

## Entities

| Entity   | Components |
|----------|------------|
| Ball     | Ball, Circle, Position, Style, Visible |
| Brick    | Brick, Position, Rectangle, Style, Visible |
| Paddle   | Input, Paddle, Position, Rectangle, Style, Visible |
| Power-Up | Circle, Position, PowerUp, Style, Velocity, Visible |
| Wall     | Position, Rectangle, Style, Visible, Wall |

## Systems

| System            | Query | Interactions |
|-------------------|-------|--------------|
| Collision Handler | | Circle, PiercingBall, Position, PowerUp, Rectangle, Style, TimedEvent, Velocity, Visible |
| Collision         | Ball, Brick, Circle, Paddle, Position, PowerUp, Rectangle, Velocity, Wall | CollisionListener<Ball, Brick>, CollisionListener<Ball, Paddle>, CollisionListener<Ball, Wall>, CollisionListener<Paddle, PowerUp>, CollisionListener<Paddle, Wall> |
| Game Over         | Ball, Position | GameOverListener |
| Input             | Input | Velocity |
| Launching         | Ball, Paddle, Position | Velocity |
| Level Loading     | | Ball, Brick, Circle, Input, Paddle, Position, Rectangle, Style, Visible, Wall |
| Movement          | Position, Velocity | |
| Rendering         | Circle, Position, Rectangle, Style, Visible | |
| Timing            | TimedEvent | |
