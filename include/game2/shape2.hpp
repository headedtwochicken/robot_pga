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
class TriangleShapeTrace : public Shape {
public:
    [[nodiscard]] std::vector<sf::Vector2f> getPoints()const  override {
        return {{400,200},{550,400},{250,400},{400,200}};
    }

    void draw(sf::RenderWindow& window) const override {

        sf::ConvexShape t;

        t.setPointCount(3);

        t.setPoint(0,{400,200});
        t.setPoint(1,{550,400});
        t.setPoint(2,{250,400});

        t.setFillColor(sf::Color::Transparent);
        t.setOutlineColor(sf::Color(120,120,120));
        t.setOutlineThickness(2);

        window.draw(t);
    }
};
class RectangularShapeTrace : public Shape {
public:
    [[nodiscard]] std::vector<sf::Vector2f> getPoints() const override {

        float w = 300;
        float h = 150;

        float x = 400;
        float y = 300;

        return {{x-w/2,y-h/2},{x+w/2,y-h/2},{x+w/2,y+h/2},{x-w/2,y+h/2},{x-w/2,y-h/2}
        };
    }

    void draw(sf::RenderWindow& window) const override {

        sf::RectangleShape r({300,150});
        r.setOrigin({150,75});
        r.setPosition({400,300});

        r.setFillColor(sf::Color::Transparent);
        r.setOutlineColor(sf::Color(120,120,120));
        r.setOutlineThickness(2);

        window.draw(r);
    }
};
class OvalShapeTrace : public Shape {
public:
    [[nodiscard]] std::vector<sf::Vector2f> getPoints() const override {
        std::vector<sf::Vector2f> pts;

        constexpr int NUM_POINTS = 100;

        for (int idx = 0; idx < NUM_POINTS; ++idx) {
            constexpr float centerX = 400.0f;
            constexpr float centerY = 300.0f;
            constexpr float radiusX = 160.0f;
            constexpr float radiusY = 90.0f;
            constexpr float TWO_PI = 6.28318530718f;

            const float t = static_cast<float>(idx) / static_cast<float>(NUM_POINTS);
            const float angle = t * TWO_PI;
            pts.emplace_back(
                centerX + std::cos(angle) * radiusX,
                centerY + std::sin(angle) * radiusY
            );
        }
        return pts;
    }

    void draw(sf::RenderWindow& window) const override {
        constexpr float centerX = 400.0f;
        constexpr float centerY = 300.0f;
        constexpr float radiusX = 160.0f;

        sf::CircleShape ellipse(radiusX);
        ellipse.setOrigin({radiusX, radiusX});
        ellipse.setPosition({centerX, centerY});
        ellipse.setScale({1.0f, 0.55f});
        ellipse.setFillColor(sf::Color::Transparent);
        ellipse.setOutlineColor(sf::Color(120, 120, 120));
        ellipse.setOutlineThickness(2.0f);
        window.draw(ellipse);
    }
};

class StarShapeTrace : public Shape {
public:
    [[nodiscard]] std::vector<sf::Vector2f> getPoints() const override {

        std::vector<sf::Vector2f> pts;

        constexpr int NUM_POINTS = 10;

        for (int idx = 0; idx < NUM_POINTS; ++idx) {
            constexpr float x = 400.0f;
            constexpr float y = 300.0f;
            constexpr float outer = 120.0f;
            constexpr float inner = 50.0f;
            constexpr float PI = 3.14159265359f;

            const float a = static_cast<float>(idx) * PI / 5.0f;
            const float r = (idx % 2 == 0) ? outer : inner;

            pts.emplace_back(
                x + std::cos(a - PI/2.0f) * r,
                y + std::sin(a - PI/2.0f) * r
            );
        }

        pts.emplace_back(pts[0]);
        return pts;
    }

    void draw(sf::RenderWindow& window) const override {

        sf::ConvexShape star;
        star.setPointCount(10);

        for (int idx = 0; idx < 10; ++idx) {
            constexpr float x = 400.0f;
            constexpr float y = 300.0f;
            constexpr float outer = 120.0f;
            constexpr float inner = 50.0f;
            constexpr float PI = 3.14159265359f;

            const float a = static_cast<float>(idx) * PI / 5.0f;
            const float r = (idx % 2 == 0) ? outer : inner;

            star.setPoint(idx, {
                x + std::cos(a - PI/2.0f) * r,
                y + std::sin(a - PI/2.0f) * r
            });
        }

        star.setFillColor(sf::Color::Transparent);
        star.setOutlineColor(sf::Color(120, 120, 120));
        star.setOutlineThickness(2.0f);

        window.draw(star);
    }
};