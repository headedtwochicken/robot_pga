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
    sf::Vector2f getPosition() const { return position; }
    void setColor(const sf::Color& c) { color = c; }
};

class FallingObject : public GameObject {
private:
    float radius;
    float speed;
    bool caught; //был ли он пойман?
    static inline std::mt19937 rng = std::mt19937{std::random_device{}()};

public:
    FallingObject(float x, float y, float r = 12.0f): GameObject(x, y), radius(r), speed(200.0f), caught(false) {
        std::uniform_int_distribution<int> dist(0, 2);
        int rnd = dist(rng);
        if (rnd == 0) color = sf::Color(255, 200, 100);// это желтоватый
        else if (rnd == 1) color = sf::Color(100, 200, 255);// это голубоватый
        else color = sf::Color(200, 100, 255);// а это розово-фиолетовый
    }

    void update(float dt) override {
        if (!caught) {
            position.y += speed * dt;
        }
    }

    void draw(sf::RenderWindow& window) const override {
        if (!caught) {
            sf::CircleShape circle(radius);
            circle.setFillColor(color);//заполнили цветом
            circle.setOutlineColor(sf::Color::White);//обводим белым
            circle.setOutlineThickness(2);//обводка-толщиной 2 пикселя
            circle.setPosition({position.x - radius, position.y - radius});
            window.draw(circle);//отправляет круг на отрисовку
        }
    }

    bool checkCatch(float catcherX, float catcherY, float catchRadius) {
        double dist = std::sqrt(std::pow(position.x - catcherX, 2) +std::pow(position.y - catcherY, 2));//dist- как distance, расстояние между центром шарика и центром ловца по т пифагора
        if (dist < radius + catchRadius && !caught) {
            caught = true;
            return true;
        }
        return false;
    }

    bool isCaught() const { return caught; }
    bool isOffScreen(float screenHeight) const { return position.y - radius > screenHeight; }// помним, чем больше число-тем ниже упал  шарик
   float getRadius() const { return radius; }
};

class Catcher {
private:
    float radius;
    sf::Color color;

public:
    Catcher() : radius(18.0f), color(sf::Color::Green) {}

    void draw(sf::RenderWindow& window, float x, float y) const {//рисуем ловец в заданных координатах
        sf::CircleShape catcher(radius);
        catcher.setFillColor(color);
        catcher.setOutlineColor(sf::Color::White);
        catcher.setOutlineThickness(3);
        catcher.setOrigin({radius, radius});
        catcher.setPosition({x, y});
        window.draw(catcher);

        sf::VertexArray claws(sf::PrimitiveType::Lines, 4);//рисуем когти для нашей ловилки на 4 точках
        claws[0].position = sf::Vector2f(x - radius, y - radius/2);
        claws[1].position = sf::Vector2f(x - radius*1.5f, y - radius);
        claws[2].position = sf::Vector2f(x + radius, y - radius/2);
        claws[3].position = sf::Vector2f(x + radius*1.5f, y - radius);

        for (int i = 0; i < 4; i++) {
            claws[i].color = sf::Color::White;//хотим белые когти
        }

        window.draw(claws);
    }

    loat getRadius() const { return radius; }
};




