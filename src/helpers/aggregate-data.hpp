#pragma once

struct CircleData {
    const Circle& body;
    const Position& position;
};

struct RectangleData {
    const Rectangle& body;
    const Position& position;

    float leftX() const {
        return position.x - body.width / 2;
    };

    float rightX() const {
        return position.x + body.width / 2;
    };

    float topY() const {
        return position.y - body.height / 2;
    };

    float bottomY() const {
        return position.y + body.height / 2;
    };
};
