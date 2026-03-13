#pragma once
#include <SFML/Graphics.hpp>
#include <random>

class GameObject {
protected:
    sf::Vector2f position;
    sf::Color color;

public:
    explicit GameObject(float x = 0, float y = 0) : position(x, y), color(sf::Color::White) {}
    virtual ~GameObject() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

    void setPosition(float x, float y) { position = {x, y}; }
    [[nodiscard]] sf::Vector2f getPosition() const { return position; }
    void setColor(const sf::Color& c) { color = c; }
};

class FallingObject : public GameObject {
private:
    float radius;
    float speed;
    bool caught;
    static inline std::mt19937 rng = std::mt19937{std::random_device{}()};

public:
    FallingObject(float x, float y, float r = 12.0f): GameObject(x, y), radius(r), speed(200.0f), caught(false) {
        std::uniform_int_distribution<int> dist(0, 2);
        int rnd = dist(rng);
        if (rnd == 0) color = sf::Color(255, 200, 100);
        else if (rnd == 1) color = sf::Color(100, 200, 255);
        else color = sf::Color(200, 100, 255);
    }

    void update(float dt) override {
        if (!caught) {
            position.y += speed * dt;
        }
    }

    void draw(sf::RenderWindow& window) const override {
        if (!caught) {
            sf::CircleShape circle(radius);
            circle.setFillColor(color);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(2);
            circle.setPosition({position.x - radius, position.y - radius});
            window.draw(circle);
        }
    }

    bool checkCatch(float catcherX, float catcherY, float catchRadius) {
        double dist = std::sqrt(std::pow(position.x - catcherX, 2) +std::pow(position.y - catcherY, 2));
        if (dist < radius + catchRadius && !caught) {
            caught = true;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool isCaught() const { return caught; }
    [[nodiscard]] bool isOffScreen(float screenHeight) const { return position.y - radius > screenHeight; }
    [[nodiscard]] float getRadius() const { return radius; }
};

class Catcher {
private:
    float radius;
    sf::Color color;

public:
    Catcher() : radius(18.0f), color(sf::Color::Green) {}

    void draw(sf::RenderWindow& window, float x, float y) const {
        sf::CircleShape catcher(radius);
        catcher.setFillColor(color);
        catcher.setOutlineColor(sf::Color::White);
        catcher.setOutlineThickness(3);
        catcher.setOrigin({radius, radius});
        catcher.setPosition({x, y});
        window.draw(catcher);

        sf::VertexArray claws(sf::PrimitiveType::Lines, 4);
        claws[0].position = sf::Vector2f(x - radius, y - radius/2);
        claws[1].position = sf::Vector2f(x - radius*1.5f, y - radius);
        claws[2].position = sf::Vector2f(x + radius, y - radius/2);
        claws[3].position = sf::Vector2f(x + radius*1.5f, y - radius);

        for (int i = 0; i < 4; i++) {
            claws[i].color = sf::Color::White;
        }

        window.draw(claws);
    }

    [[nodiscard]] float getRadius() const { return radius; }
};




