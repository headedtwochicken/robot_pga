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

    void addAngles(float dTheta1, float dTheta2) {
        theta1 += dTheta1;
        theta2 += dTheta2;
        updateKinematics();
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