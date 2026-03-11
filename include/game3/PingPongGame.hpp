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

    void update(sf::RenderWindow& window) override {
        const float dt = clock.restart().asSeconds();
        const float dt60 = std::clamp(dt * 60.0f, 0.35f, 2.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Q)) {
            racketTilt -= 0.09f * dt60;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::E)) {
            racketTilt += 0.09f * dt60;
        }

        const bool rNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R);
        if (rNow && !prevR) {
            restartNow();
        }
        prevR = rNow;

        const bool spaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space);
        if (std::holds_alternative<WaitingStartState>(screen) && spaceNow && !prevSpace) {
            startRound();
        }
        prevSpace = spaceNow;

        const bool mouseLeftNow = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        const sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if (restartButton) {
            restartButton->setHovered(restartButton->contains(mousePosF));

            if (mouseLeftNow && !prevMouseLeft && restartButton->contains(mousePosF)) {
                restartButton->setPressed(true);
            }

            if (!mouseLeftNow && prevMouseLeft) {
                if (restartButton->contains(mousePosF)) {
                    restartNow();
                }
                restartButton->setPressed(false);
            }
        }
        prevMouseLeft = mouseLeftNow;

        paddleHitCooldown = std::max(0.0f, paddleHitCooldown - dt60);
        racketTilt = std::clamp(racketTilt, -0.68f, 0.68f);

        const float desiredBase = std::clamp(static_cast<float>(mousePos.x), 100.0f, PLAY_W - 100.0f);
        baseX += (desiredBase - baseX) * 0.24f * std::min(dt60, 1.0f);

        const float targetX = std::clamp(static_cast<float>(mousePos.x), WALL_PAD + 32.0f, PLAY_W - WALL_PAD - 32.0f);
        const float targetY = std::clamp(static_cast<float>(mousePos.y), TOP_PAD + 30.0f, BASE_Y - 50.0f);

        const bool elbowUp = targetX >= baseX;
        if (!solveIJOlya(targetX - baseX, targetY - BASE_Y, L1, L2, theta1, theta2, elbowUp)) {
            const float clampedX = std::clamp(targetX - baseX, -(L1 + L2 - 10.0f), L1 + L2 - 10.0f);
            const float clampedY = std::clamp(targetY - BASE_Y, -(L1 + L2 - 10.0f), 0.0f);
            theta1 = std::atan2(clampedY, clampedX);
            theta2 = 0.0f;
        }

        const Multivector origin = makePoint(0.0f, 0.0f);
        const Multivector mBase = makeTranslator(baseX, BASE_Y);
        const Multivector mJoint1 = mBase * makeRotor(theta1);
        const Multivector mJoint2 = mJoint1 * makeTranslator(L1, 0.0f) * makeRotor(theta2);
        const Multivector mEnd = mJoint2 * makeTranslator(L2, 0.0f);

        const PongVec2 endPoint = pointFromMultivector(applyMotor(mEnd, origin));
        const Multivector racketMotor = makeTranslator(endPoint.x, endPoint.y) * makeRotor(racketTilt);

        const PongVec2 racketA = pointFromMultivector(applyMotor(racketMotor, makePoint(-RACKET_HALF, 0.0f)));
        const PongVec2 racketB = pointFromMultivector(applyMotor(racketMotor, makePoint( RACKET_HALF, 0.0f)));
        const PongVec2 racketCenter = (racketA + racketB) * 0.5f;

        if (!racketCenterPrev.has_value()) {
            racketCenterPrev = racketCenter;
        }
        const PongVec2 racketVelocity = (racketCenter - *racketCenterPrev) / std::max(dt60, 0.001f);
        racketCenterPrev = racketCenter;

        if (std::holds_alternative<PlayingState>(screen)) {
            const int substeps = std::clamp(static_cast<int>(std::ceil(length(ball->vel) * dt60 / 2.5f)), 2, 10);
            const float subDt = dt60 / static_cast<float>(substeps);

            for (int i = 0; i < substeps && std::holds_alternative<PlayingState>(screen); ++i) {
                ball->step(subDt);
                PongVec2 v = ball->vel;

                if (ball->pos.x - ball->radius < WALL_PAD) {
                    ball->pos.x = WALL_PAD + ball->radius;
                    v.x = std::abs(v.x);
                }
                if (ball->pos.x + ball->radius > PLAY_W - WALL_PAD) {
                    ball->pos.x = PLAY_W - WALL_PAD - ball->radius;
                    v.x = -std::abs(v.x);
                }
                if (ball->pos.y - ball->radius < TOP_PAD) {
                    ball->pos.y = TOP_PAD + ball->radius;
                    v.y = std::abs(v.y);
                }
                ball->vel = v;

                const PongVec2 closest = closestPointOnSegment(racketA, racketB, ball->pos);
                const PongVec2 delta = ball->pos - closest;
                const float d2 = lengthSq(delta);
                const bool downward = v.y > 0.0f;
                const bool fromAbove = ball->prevPos.y <= std::max(racketA.y, racketB.y) + COLLISION_RADIUS + 4.0f;

                if (downward && fromAbove && d2 <= COLLISION_RADIUS * COLLISION_RADIUS && paddleHitCooldown <= 0.0f) {
                    PongVec2 tangent = normalize(racketB - racketA);
                    if (lengthSq(tangent) < 1e-6f) {
                        tangent = {1.0f, 0.0f};
                    }

                    PongVec2 upNormal = {-tangent.y, tangent.x};
                    if (upNormal.y > 0.0f) {
                        upNormal = upNormal * -1.0f;
                    }
                    if (dot(v, upNormal) > -0.02f) {
                        upNormal = upNormal * -1.0f;
                    }
                    if (upNormal.y > -0.08f) {
                        upNormal.y = -0.08f;
                        upNormal = normalize(upNormal);
                    }

                    const float hitOffset = std::clamp(dot(ball->pos - racketCenter, tangent) / RACKET_HALF, -1.0f, 1.0f);
                    const float tangentBoost = hitOffset * 1.15f + racketVelocity.x * 0.07f + racketTilt * 0.65f;

                    PongVec2 outDir = normalize(upNormal * 1.25f + tangent * tangentBoost);
                    if (outDir.y > -0.25f) {
                        outDir.y = -0.25f;
                        outDir = normalize(outDir);
                    }

                    const float speed = std::clamp(length(v) + 0.24f, 7.2f, 12.8f);
                    PongVec2 bounced = outDir * speed;
                    bounced.x = clampAbsMin(bounced.x, 1.55f);
                    bounced.y = -std::max(std::abs(bounced.y), 3.3f);

                    const float dist = std::sqrt(std::max(d2, 1e-6f));
                    ball->pos = closest + upNormal * (COLLISION_RADIUS + 1.8f + std::max(0.0f, COLLISION_RADIUS - dist));
                    ball->vel = bounced;
                    ball->visualSpin = std::clamp(hitOffset * 0.28f + racketTilt * 0.55f, -0.45f, 0.45f);
                    paddleHitCooldown = 6.0f;
                    ++score;
                }

                if (length(ball->vel) < 6.8f) {
                    ball->vel = normalize(ball->vel) * 6.8f;
                }
                ball->vel.x = clampAbsMin(ball->vel.x, 1.25f);
                if (std::abs(ball->vel.y) < 2.6f) {
                    ball->vel.y = (ball->vel.y < 0.0f ? -1.0f : 1.0f) * 2.6f;
                }

                if (ball->pos.y - ball->radius > WINDOW_H) {
                    screen = GameOverState{score};
                }

                ball->syncMotor();
            }
        }
    }

    void draw(sf::RenderWindow& window) override {
        const Multivector origin = makePoint(0.0f, 0.0f);
        const Multivector mBase = makeTranslator(baseX, BASE_Y);
        const Multivector mJoint1 = mBase * makeRotor(theta1);
        const Multivector mJoint2 = mJoint1 * makeTranslator(L1, 0.0f) * makeRotor(theta2);
        const Multivector mEnd = mJoint2 * makeTranslator(L2, 0.0f);

        const PongVec2 base = pointFromMultivector(applyMotor(mBase, origin));
        const PongVec2 joint = pointFromMultivector(applyMotor(mJoint1 * makeTranslator(L1, 0.0f), origin));
        const PongVec2 end = pointFromMultivector(applyMotor(mEnd, origin));

        PongRenderSnapshot snapshot;
        snapshot.playW = PLAY_W;
        snapshot.hudW = HUD_W;
        snapshot.windowH = static_cast<float>(WINDOW_H);
        snapshot.wallPad = WALL_PAD;
        snapshot.topPad = TOP_PAD;
        snapshot.baseY = BASE_Y;
        snapshot.racketHalf = RACKET_HALF;
        snapshot.racketThickness = RACKET_THICKNESS;
        snapshot.base = base;
        snapshot.joint = joint;
        snapshot.end = end;
        snapshot.racketTilt = racketTilt;
        snapshot.ballPos = ball->pos;
        snapshot.ballRadius = ball->radius;
        snapshot.ballVisualAngle = ball->visualAngle;
        snapshot.score = score;
        snapshot.screen = screen;
        snapshot.font = bitmapFont;
        snapshot.restartButton = restartButton.get();
        snapshot.hudWidgets = &hudWidgets;

        PingPongGameView::draw(window, snapshot);
    }

    [[nodiscard]] GameResult getResult() const override {
        return score;
    }
};