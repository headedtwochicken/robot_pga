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
