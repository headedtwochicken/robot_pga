#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;

    [[nodiscard]] virtual std::vector<sf::Vector2f> getPoints() const = 0;

    virtual void draw(sf::RenderWindow& window) const = 0;
};
class CircleShapeTrace : public Shape {

    float x = 400.0f;
    float y = 300.0f;
    float r = 120.0f;
    static constexpr float TWO_PI = 6.28318530718f;

public:
    [[nodiscard]] std::vector<sf::Vector2f> getPoints() const override {
        std::vector<sf::Vector2f> pts;
        constexpr int NUM_POINTS = 100;

        for (int idx = 0; idx < NUM_POINTS; ++idx) {
            const float t = static_cast<float>(idx) / static_cast<float>(NUM_POINTS);
            const float a = t * TWO_PI;
            pts.emplace_back(
                x + std::cos(a) * r,
                y + std::sin(a) * r
            );
        }
        return pts;
    }

    void draw(sf::RenderWindow& window) const override {

        sf::CircleShape c(r);
        c.setOrigin({r, r});
        c.setPosition({x, y});

        c.setFillColor(sf::Color::Transparent);
        c.setOutlineColor(sf::Color(120,120,120));
        c.setOutlineThickness(2);

        window.draw(c);
    }
};

class SquareShapeTrace : public Shape {
public:
    [[nodiscard]] std::vector<sf::Vector2f> getPoints() const override {
        float s = 200;

        float x = 400;
        float y = 300;
        return {
                    {x-s/2,y-s/2},
                    {x+s/2,y-s/2},
                    {x+s/2,y+s/2},
                    {x-s/2,y+s/2},
                    {x-s/2,y-s/2}
        };
    }

    void draw(sf::RenderWindow& window) const override {

        sf::RectangleShape r({200,200});
        r.setOrigin({100,100});
        r.setPosition({400,300});

        r.setFillColor(sf::Color::Transparent);
        r.setOutlineColor(sf::Color(120,120,120));
        r.setOutlineThickness(2);

        window.draw(r);
    }
};
