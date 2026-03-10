#pragma once
#include <SFML/Graphics.hpp>
#include "pga2d.hpp"
#include <cmath>
#include <algorithm>
#include <optional>

class RobotArm {
private:
    float baseX, baseY;
    float L1, L2;
    float theta1, theta2;
    float w1, w2;
    float j0x, j0y, j1x, j1y, j2x, j2y;
    static constexpr float GRAVITY = 0.05f;

    void updateKinematics() {
        j0x = baseX;
        j0y = baseY;
    }

public:
    RobotArm(float x, float y, float len1, float len2):
    baseX(x), baseY(y), L1(len1), L2(len2), theta1(0.0f), theta2(0.5f), w1(0.0f), w2(0.0f),j0x(0.0f), j0y(0.0f), j1x(0.0f), j1y(0.0f), j2x(0.0f), j2y(0.0f) {
        updateKinematics();
    }

    [[nodiscard]] float getEndX() const { return j2x; }
    [[nodiscard]] float getEndY() const { return j2y; }
    [[nodiscard]] float getBaseY() const { return baseY; }

    void resetVelocities() { w1 = 0.0f; w2 = 0.0f; }

    void moveBase(float dx, float dy = 0.0f) {
        baseX += dx;
        baseY += dy;
        updateKinematics();
    }

    void draw(sf::RenderWindow& window) const {
    }
};