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
        Multivector origin = makePoint(0.0f, 0.0f);
        Multivector motorBase = makeTranslator(baseX, baseY);

        Multivector m_joint1 = motorBase;
        Multivector m_link1  = m_joint1 * makeRotor(theta1);

        Multivector m_joint2 = m_link1 * makeTranslator(L1, 0.0f);
        Multivector m_link2  = m_joint2 * makeRotor(theta2);

        Multivector m_end = m_link2 * makeTranslator(L2, 0.0f);

        getPoint(applyMotor(m_joint1, origin), j0x, j0y);
        getPoint(applyMotor(m_joint2, origin), j1x, j1y);
        getPoint(applyMotor(m_end, origin), j2x, j2y);
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

    void clampBaseY(float floorY) {
        if (baseY > floorY) baseY = floorY;
        updateKinematics();
    }

    void addAngles(float dTheta1, float dTheta2) {
        theta1 += dTheta1;
        theta2 += dTheta2;
        updateKinematics();
    }

    [[nodiscard]] std::optional<float> solveIK(float targetX, float targetY) {
        float mx = targetX - baseX;
        float my = targetY - baseY;
        float d2 = mx * mx + my * my;
        float d = std::sqrt(d2);

        if (d >= L1 + L2 - 1.0f) {
            theta1 = std::atan2(my, mx);
            theta2 = 0.0f;
            updateKinematics();
            return std::nullopt;
        }

        if (d <= std::abs(L1 - L2) + 1.0f) {
            theta1 = std::atan2(my, mx);
            theta2 = 3.14159265f;
            updateKinematics();
            return std::nullopt;
        }

        float cosTheta2 = (d2 - L1 * L1 - L2 * L2) / (2 * L1 * L2);
        cosTheta2 = std::max(-1.0f, std::min(1.0f, cosTheta2));
        theta2 = std::acos(cosTheta2);

        float alpha = std::atan2(my, mx);
        float beta = std::atan2(L2 * std::sin(theta2), L1 + L2 * std::cos(theta2));
        theta1 = alpha - beta;

        updateKinematics();
        return d;
    }

    // === ФИЗИКА И СТОЛКНОВЕНИЯ ===
    void updatePhysics(bool checkCollisions, float floorY = 400.0f) {
        Multivector origin = makePoint(0.0f, 0.0f);
        Multivector motorBase = makeTranslator(baseX, baseY);
        Multivector m_link1 = motorBase * makeRotor(theta1);
        Multivector m_link2 = m_link1 * makeTranslator(L1, 0.0f) * makeRotor(theta2);

        Multivector com1 = applyMotor(m_link1 * makeTranslator(L1 * 0.5f, 0.0f), origin);
        Multivector com2 = applyMotor(m_link2 * makeTranslator(L2 * 0.5f, 0.0f), origin);

        float c1x, c1y, c2x, c2y;
        getPoint(com1, c1x, c1y);
        getPoint(com2, c2x, c2y);

        // Базовая гравитация
        float torque2 = (c2x - j1x) * GRAVITY;
        float torque1 = (c1x - j0x) * GRAVITY + torque2;

        // === РЕАКЦИЯ ОПОРЫ ===
        if (checkCollisions) {
            constexpr float RESTITUTION = 0.1f;
            // 1. Проверяем ЛОКОТЬ (желтую точку)
            if (j1y >= floorY) {
                float vy1 = w1 * (j1x - j0x);
                if (vy1 > 0.0f) {
                    float r1 = j1x - j0x;
                    float effMass = r1 * r1;
                    if (effMass > 0.0001f) {
                        float impulse = -(1.0f + RESTITUTION) * vy1 / effMass;
                        w1 += impulse * r1;
                    }
                }
                float penetration = j1y - floorY;
                theta1 -= (penetration / std::abs(j1x - j0x + 0.001f)) * static_cast<float>(j1x >= j0x ? 1 : -1);
                w1 *= 0.5f;
            }

            // 2. Проверяем КОНЕЦ РУКИ (зеленую точку)
            if (j2y >= floorY) {
                float r1 = j2x - j0x;
                float r2 = j2x - j1x;
                float vy2 = w1 * r1 + w2 * r2;

                if (vy2 > 0.0f) {
                    float effMass = (r1 * r1) + (r2 * r2);
                    if (effMass > 0.0001f) {
                        float impulse = -(1.0f + RESTITUTION) * vy2 / effMass;
                        w1 += impulse * r1;
                        w2 += impulse * r2;
                    }
                }
                float penetration = j2y - floorY;
                theta2 -= (penetration / std::abs(r2 + 0.001f)) * static_cast<float>(j2x >= j1x ? 1 : -1);
                w2 *= 0.5f;
                w1 *= 0.8f;
            }
        }

        w2 = (w2 + torque2 * 0.01f) * 0.98f;
        w1 = (w1 + torque1 * 0.01f) * 0.98f;

        theta2 += w2;
        theta1 += w1;
        updateKinematics();
    }

    void grabWithMouse(float mouseX, float mouseY, bool checkCollisions, float floorY = 400.0f) {
        if (checkCollisions && mouseY > floorY) mouseY = floorY;

        float forceX = (mouseX - j2x) * 0.02f;
        float forceY = (mouseY - j2y) * 0.02f;

        float torquePull2 = (j2x - j1x) * forceY - (j2y - j1y) * forceX;
        float torquePull1 = (j2x - j0x) * forceY - (j2y - j0y) * forceX;

        w2 += torquePull2 * 0.01f;
        w1 += torquePull1 * 0.01f;
    }

    void draw(sf::RenderWindow& window) const {
        sf::VertexArray link1(sf::PrimitiveType::Lines, 2);
        link1[0].position = sf::Vector2f(j0x, j0y); link1[1].position = sf::Vector2f(j1x, j1y);
        link1[0].color = sf::Color::White; link1[1].color = sf::Color::White;
        window.draw(link1);

        sf::VertexArray link2(sf::PrimitiveType::Lines, 2);
        link2[0].position = sf::Vector2f(j1x, j1y); link2[1].position = sf::Vector2f(j2x, j2y);
        link2[0].color = sf::Color::White; link2[1].color = sf::Color::Cyan;
        window.draw(link2);

        sf::CircleShape joint(5);
        joint.setOrigin({5, 5});

        joint.setFillColor(sf::Color::Red); joint.setPosition({j0x, j0y}); window.draw(joint);
        joint.setFillColor(sf::Color::Yellow); joint.setPosition({j1x, j1y}); window.draw(joint);
        joint.setFillColor(sf::Color::Green); joint.setPosition({j2x, j2y}); window.draw(joint);
    }
};