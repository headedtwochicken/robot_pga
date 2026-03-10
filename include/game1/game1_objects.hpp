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

inline std::optional<std::pair<float, float>> solveIK(
    float targetX, float targetY,
    float baseX, float baseY,
    float L1, float L2) {
    float mx = targetX - baseX;
    float my = targetY - baseY;

    float d2 = mx * mx + my * my;
    float d = std::sqrt(d2);

    if (d >= L1 + L2 - 10.0f) {
        float theta1 = std::atan2(my, mx);
        return std::make_pair(theta1, 0.0f);
    }

    if (d <= std::abs(L1 - L2) + 10.0f) {
        float theta1 = std::atan2(my, mx);
        return std::make_pair(theta1, 0.0f);
    }

    float cosTheta2 = (d2 - L1*L1 - L2*L2) / (2 * L1 * L2);
    cosTheta2 = std::max(-1.0f, std::min(1.0f, cosTheta2));
    float theta2 = std::acos(cosTheta2);

    float alpha = std::atan2(my, mx);
    float beta = std::atan2(L2 * std::sin(theta2), L1 + L2 * std::cos(theta2));
    float theta1 = alpha - beta;

    return std::make_pair(theta1, theta2);
}

