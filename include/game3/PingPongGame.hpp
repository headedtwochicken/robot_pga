#pragma once

#include <SFML/Graphics.hpp>
#include "../game2/BaseGame2.hpp"
#include "../pga2d.hpp"
#include "PingPongGameView.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

class BallEntity final {
public:
    Multivector motor;
    PongVec2 pos{};
    PongVec2 prevPos{};
    PongVec2 vel{};
    float radius = 11.0f;
    float visualAngle = 0.0f;
    float visualSpin = 0.0f;

    void syncMotor() {
        motor = makeTranslator(pos.x, pos.y);
    }

    void launch(float x, float y) {
        pos = {x, y};
        prevPos = pos;
        vel = {5.6f, 6.1f};
        visualAngle = 0.0f;
        visualSpin = 0.0f;
        syncMotor();
    }

    void park(float x, float y) {
        pos = {x, y};
        prevPos = pos;
        vel = {0.0f, 0.0f};
        visualAngle = 0.0f;
        visualSpin = 0.0f;
        syncMotor();
    }

    void step(float dt60) {
        prevPos = pos;
        pos += vel * dt60;
        visualAngle += visualSpin * dt60;
        visualSpin *= std::clamp(1.0f - 0.045f * dt60, 0.0f, 1.0f);
        syncMotor();
    }
};

class PingPongGame : public BaseGame {
private:
    static constexpr unsigned WINDOW_W = 1120;
    static constexpr unsigned WINDOW_H = 700;
    static constexpr float HUD_W = 220.0f;
    static constexpr float PLAY_W = WINDOW_W - HUD_W;

    static constexpr float WALL_PAD = 22.0f;
    static constexpr float TOP_PAD = 28.0f;
    static constexpr float L1 = 120.0f;
    static constexpr float L2 = 100.0f;
    static constexpr float RACKET_HALF = 112.0f;
    static constexpr float RACKET_THICKNESS = 20.0f;
    static constexpr float BASE_Y = WINDOW_H - 46.0f;
    static constexpr float COLLISION_RADIUS = 11.0f + RACKET_THICKNESS * 0.78f;

    static float dot(const PongVec2& a, const PongVec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    static float lengthSq(const PongVec2& v) {
        return dot(v, v);
    }

    static float length(const PongVec2& v) {
        return std::sqrt(lengthSq(v));
    }

    static PongVec2 normalize(const PongVec2& v) {
        const float len = length(v);
        return (len < 1e-6f) ? PongVec2{0.0f, 0.0f} : v / len;
    }

    static PongVec2 pointFromMultivector(const Multivector& p) {
        float x = 0.0f;
        float y = 0.0f;
        getPoint(p, x, y);
        return {x, y};
    }

    static PongVec2 closestPointOnSegment(const PongVec2& a, const PongVec2& b, const PongVec2& p) {
        const PongVec2 ab = b - a;
        const float abLenSq = lengthSq(ab);
        if (abLenSq < 1e-6f) {
            return a;
        }
        const float t = std::clamp(dot(p - a, ab) / abLenSq, 0.0f, 1.0f);
        return a + ab * t;
    }

    static float clampAbsMin(float v, float minAbs) {
        if (std::abs(v) >= minAbs) {
            return v;
        }
        return (v < 0.0f ? -minAbs : minAbs);
    }

    static bool solveIJOlya(float targetX,
                            float targetY,
                            float len1,
                            float len2,
                            float& t1,
                            float& t2,
                            bool elbowUp = true) {
        const float d2 = targetX * targetX + targetY * targetY;
        const float d = std::sqrt(d2);
        if (d > len1 + len2 || d < std::abs(len1 - len2)) {
            return false;
        }

        const float cosTheta2 = std::clamp(
            (d2 - len1 * len1 - len2 * len2) / (2.0f * len1 * len2),
            -1.0f,
            1.0f
        );

        t2 = elbowUp ? std::acos(cosTheta2) : -std::acos(cosTheta2);

        const float alpha = std::atan2(targetY, targetX);
        const float beta = std::atan2(len2 * std::sin(t2), len1 + len2 * std::cos(t2));
        t1 = alpha - beta;
        return true;
    }

    sf::Clock clock;

    float baseX = PLAY_W * 0.5f;
    float theta1 = -1.2f;
    float theta2 = 1.15f;
    float racketTilt = 0.0f;

    std::optional<PongVec2> racketCenterPrev;
    ScreenState screen = WaitingStartState{};

    std::shared_ptr<PixelBitmapFont> bitmapFont;
    std::unique_ptr<BallEntity> ball;

    std::unique_ptr<RestartButtonWidget> restartButton;
    std::vector<std::unique_ptr<UiWidget>> hudWidgets;

    int score = 0;
    float paddleHitCooldown = 0.0f;

    bool prevR = false;
    bool prevSpace = false;
    bool prevMouseLeft = false;

    void setupScene() {
        score = 0;
        screen = WaitingStartState{};
        baseX = PLAY_W * 0.5f;
        theta1 = -1.2f;
        theta2 = 1.15f;
        racketTilt = 0.0f;
        ball->park(PLAY_W * 0.5f, 155.0f);
        racketCenterPrev.reset();
        paddleHitCooldown = 0.0f;
        if (restartButton) {
            restartButton->setPressed(false);
        }
    }

    void startRound() {
        score = 0;
        screen = PlayingState{};
        ball->launch(PLAY_W * 0.5f, 155.0f);
        paddleHitCooldown = 0.0f;
    }

    void restartNow() {
        setupScene();
        screen = PlayingState{};
        ball->launch(PLAY_W * 0.5f, 155.0f);
    }

public:
    PingPongGame() try
        : bitmapFont(std::make_shared<PixelBitmapFont>()),
          ball(std::make_unique<BallEntity>()) {
        restartButton = std::make_unique<RestartButtonWidget>(
            sf::FloatRect({PLAY_W + 24.0f, 508.0f}, {HUD_W - 48.0f, 58.0f}),
            bitmapFont
        );

        hudWidgets.push_back(std::make_unique<SmileyWidget>(
            PLAY_W + HUD_W * 0.5f - 25.0f,
            427.0f,
            5.0f,
            bitmapFont
        ));

        setupScene();
    } catch (const std::invalid_argument& ex) {
        throw std::runtime_error(std::string("PingPongGame initialization failed: ") + ex.what());
    }

    void update(sf::RenderWindow& window) override {}

    void draw(sf::RenderWindow& window) override {}
};