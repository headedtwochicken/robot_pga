#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

struct PongVec2 {
    float x = 0.0f;
    float y = 0.0f;

    PongVec2 operator+(const PongVec2& rhs) const { return {x + rhs.x, y + rhs.y}; }
    PongVec2 operator-(const PongVec2& rhs) const { return {x - rhs.x, y - rhs.y}; }
    PongVec2 operator*(float s) const { return {x * s, y * s}; }
    PongVec2 operator/(float s) const { return {x / s, y / s}; }
    PongVec2& operator+=(const PongVec2& rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

struct WaitingStartState {};
struct PlayingState {};
struct GameOverState {
    int finalScore = 0;
};

using ScreenState = std::variant<WaitingStartState, PlayingState, GameOverState>;

class PixelBitmapFont final {
public:
    [[nodiscard]] float textWidth(const std::string& text, float pixelSize) const {
        if (text.empty()) {
            return 0.0f;
        }
        return pixelSize * (static_cast<float>(text.size()) * glyphAdvance_ - 1.0f);
    }

    void drawText(sf::RenderTarget& target,
                  const std::string& text,
                  float x,
                  float y,
                  float pixelSize,
                  sf::Color color) const {
        sf::RectangleShape pixel({pixelSize, pixelSize});
        pixel.setFillColor(color);

        float cursorX = x;
        for (char c : text) {
            const auto& pat = glyph(c);
            for (std::size_t row = 0; row < pat.size(); ++row) {
                for (int col = 0; col < 5; ++col) {
                    if (pat[row][col] == '1') {
                        pixel.setPosition({
                            cursorX + static_cast<float>(col) * pixelSize,
                            y + static_cast<float>(row) * pixelSize
                        });
                        target.draw(pixel);
                    }
                }
            }
            cursorX += pixelSize * glyphAdvance_;
        }
    }

    void drawTextCentered(sf::RenderTarget& target,
                          const std::string& text,
                          float centerX,
                          float y,
                          float pixelSize,
                          sf::Color color) const {
        drawText(target, text, centerX - textWidth(text, pixelSize) * 0.5f, y, pixelSize, color);
    }

    static void drawPixelSmiley(sf::RenderTarget& target,
                         float x,
                         float y,
                         float pixelSize,
                         sf::Color faceColor,
                         sf::Color featureColor)  {
        static constexpr std::array<const char*, 10> pattern = {
            "0011111100", "0111111110", "1111111111", "1110010011",
            "1110010011", "1111111111", "1100000001", "1110000011",
            "0111111110", "0011111100"
        };

        sf::RectangleShape pixel({pixelSize, pixelSize});
        for (std::size_t row = 0; row < pattern.size(); ++row) {
            for (int col = 0; col < 10; ++col) {
                if (pattern[row][col] == '0') {
                    continue;
                }

                const bool feature =
                    ((row == 3 || row == 4) && (col == 3 || col == 4 || col == 6 || col == 7)) ||
                    (row == 6 && col >= 2 && col <= 7) ||
                    (row == 7 && col >= 3 && col <= 6);

                pixel.setFillColor(feature ? featureColor : faceColor);
                pixel.setPosition({
                    x + static_cast<float>(col) * pixelSize,
                    y + static_cast<float>(row) * pixelSize
                });
                target.draw(pixel);
            }
        }
    }

private:
    [[nodiscard]] static const std::array<const char*, 7>& glyph(char c) {
        static constexpr std::array<const char*, 7> blank = {"00000","00000","00000","00000","00000","00000","00000"};
        static constexpr std::array<const char*, 7> A = {"01110","10001","10001","11111","10001","10001","10001"};
        static constexpr std::array<const char*, 7> B = {"11110","10001","10001","11110","10001","10001","11110"};
        static constexpr std::array<const char*, 7> C = {"01111","10000","10000","10000","10000","10000","01111"};
        static constexpr std::array<const char*, 7> D = {"11110","10001","10001","10001","10001","10001","11110"};
        static constexpr std::array<const char*, 7> E = {"11111","10000","10000","11110","10000","10000","11111"};
        static constexpr std::array<const char*, 7> F = {"11111","10000","10000","11110","10000","10000","10000"};
        static constexpr std::array<const char*, 7> G = {"01111","10000","10000","10111","10001","10001","01111"};
        static constexpr std::array<const char*, 7> H = {"10001","10001","10001","11111","10001","10001","10001"};
        static constexpr std::array<const char*, 7> I = {"11111","00100","00100","00100","00100","00100","11111"};
        static constexpr std::array<const char*, 7> K = {"10001","10010","10100","11000","10100","10010","10001"};
        static constexpr std::array<const char*, 7> L = {"10000","10000","10000","10000","10000","10000","11111"};
        static constexpr std::array<const char*, 7> M = {"10001","11011","10101","10101","10001","10001","10001"};
        static constexpr std::array<const char*, 7> N = {"10001","11001","10101","10011","10001","10001","10001"};
        static constexpr std::array<const char*, 7> O = {"01110","10001","10001","10001","10001","10001","01110"};
        static constexpr std::array<const char*, 7> P = {"11110","10001","10001","11110","10000","10000","10000"};
        static constexpr std::array<const char*, 7> R = {"11110","10001","10001","11110","10100","10010","10001"};
        static constexpr std::array<const char*, 7> S = {"01111","10000","10000","01110","00001","00001","11110"};
        static constexpr std::array<const char*, 7> T = {"11111","00100","00100","00100","00100","00100","00100"};
        static constexpr std::array<const char*, 7> U = {"10001","10001","10001","10001","10001","10001","01110"};
        static constexpr std::array<const char*, 7> V = {"10001","10001","10001","10001","10001","01010","00100"};
        static constexpr std::array<const char*, 7> W = {"10001","10001","10001","10101","10101","11011","10001"};
        static constexpr std::array<const char*, 7> Y = {"10001","10001","01010","00100","00100","00100","00100"};
        static constexpr std::array<const char*, 7> zero = {"01110","10001","10011","10101","11001","10001","01110"};
        static constexpr std::array<const char*, 7> one = {"00100","01100","00100","00100","00100","00100","01110"};
        static constexpr std::array<const char*, 7> two = {"01110","10001","00001","00010","00100","01000","11111"};
        static constexpr std::array<const char*, 7> three = {"11110","00001","00001","01110","00001","00001","11110"};
        static constexpr std::array<const char*, 7> four = {"00010","00110","01010","10010","11111","00010","00010"};
        static constexpr std::array<const char*, 7> five = {"11111","10000","10000","11110","00001","00001","11110"};
        static constexpr std::array<const char*, 7> six = {"01110","10000","10000","11110","10001","10001","01110"};
        static constexpr std::array<const char*, 7> seven = {"11111","00001","00010","00100","01000","01000","01000"};
        static constexpr std::array<const char*, 7> eight = {"01110","10001","10001","01110","10001","10001","01110"};
        static constexpr std::array<const char*, 7> nine = {"01110","10001","10001","01111","00001","00001","01110"};

        switch (std::toupper(static_cast<unsigned char>(c))) {
            case 'A': return A; case 'B': return B; case 'C': return C; case 'D': return D;
            case 'E': return E; case 'F': return F; case 'G': return G; case 'H': return H;
            case 'I': return I; case 'K': return K; case 'L': return L; case 'M': return M;
            case 'N': return N; case 'O': return O; case 'P': return P; case 'R': return R;
            case 'S': return S; case 'T': return T; case 'U': return U; case 'V': return V;
            case 'W': return W; case 'Y': return Y; case '0': return zero; case '1': return one;
            case '2': return two; case '3': return three; case '4': return four; case '5': return five;
            case '6': return six; case '7': return seven; case '8': return eight; case '9': return nine;
            default: return blank;
        }
    }

    float glyphAdvance_ = 6.0f;
};

class UiWidget {
public:
    virtual ~UiWidget() = default;
    virtual void draw(sf::RenderTarget& target) const = 0;
};

class SmileyWidget final : public UiWidget {
public:
    SmileyWidget(float x, float y, float pixelSize, std::shared_ptr<PixelBitmapFont> font)
        : x_(x), y_(y), pixelSize_(pixelSize), font_(std::move(font)) {
        if (!font_) {
            throw std::invalid_argument("SmileyWidget requires a shared PixelBitmapFont");
        }
    }

    void draw(sf::RenderTarget& target) const override {
        PixelBitmapFont::drawPixelSmiley(
            target,
            x_,
            y_,
            pixelSize_,
            sf::Color(255, 218, 92),
            sf::Color(19, 30, 45)
        );
    }

private:
    float x_ = 0.0f;
    float y_ = 0.0f;
    float pixelSize_ = 1.0f;
    std::shared_ptr<PixelBitmapFont> font_;
};

class RestartButtonWidget final {
public:
    RestartButtonWidget(sf::FloatRect rect, std::shared_ptr<PixelBitmapFont> font)
        : rect_(rect), font_(std::move(font)) {
        if (!font_) {
            throw std::invalid_argument("RestartButtonWidget requires a shared PixelBitmapFont");
        }
    }

    void setHovered(bool value) { hovered_ = value; }
    void setPressed(bool value) { pressed_ = value; }

    [[nodiscard]] bool contains(sf::Vector2f p) const { return rect_.contains(p); }

    void draw(sf::RenderTarget& target) const {
        sf::RectangleShape restartButton({rect_.size.x, rect_.size.y});
        restartButton.setPosition({rect_.position.x, rect_.position.y});
        restartButton.setFillColor(
            pressed_ ? sf::Color(40, 110, 185)
                     : (hovered_ ? sf::Color(32, 92, 160)
                                 : sf::Color(24, 70, 122)));
        restartButton.setOutlineThickness(2.0f);
        restartButton.setOutlineColor(sf::Color(160, 220, 255, 180));
        target.draw(restartButton);

        font_->drawTextCentered(
            target,
            "RESTART",
            rect_.position.x + rect_.size.x * 0.5f,
            rect_.position.y + 18.0f,
            4.0f,
            sf::Color(240, 247, 255)
        );
    }

private:
    sf::FloatRect rect_{};
    bool hovered_ = false;
    bool pressed_ = false;
    std::shared_ptr<PixelBitmapFont> font_;
};

struct PongRenderSnapshot {
    float playW = 0.0f;
    float hudW = 0.0f;
    float windowH = 0.0f;
    float wallPad = 0.0f;
    float topPad = 0.0f;
    float baseY = 0.0f;
    float racketHalf = 0.0f;
    float racketThickness = 0.0f;

    PongVec2 base{};
    PongVec2 joint{};
    PongVec2 end{};
    float racketTilt = 0.0f;

    PongVec2 ballPos{};
    float ballRadius = 0.0f;
    float ballVisualAngle = 0.0f;

    int score = 0;
    ScreenState screen{WaitingStartState{}};

    std::shared_ptr<PixelBitmapFont> font;
    const RestartButtonWidget* restartButton = nullptr;
    const std::vector<std::unique_ptr<UiWidget>>* hudWidgets = nullptr;
};

class PingPongGameView final {
public:
    static void draw(sf::RenderWindow& window, const PongRenderSnapshot& s) {
        if (!s.font) {
            return;
        }

        window.clear(sf::Color(8, 14, 24));

        sf::RectangleShape playBackground({s.playW, s.windowH});
        playBackground.setFillColor(sf::Color(8, 14, 24));
        window.draw(playBackground);

        sf::RectangleShape wallTop({s.playW - 2.0f * s.wallPad, 3.0f});
        wallTop.setPosition({s.wallPad, s.topPad});
        wallTop.setFillColor(sf::Color(65, 115, 180, 190));
        window.draw(wallTop);

        sf::RectangleShape wallLeft({3.0f, s.windowH - s.topPad - 40.0f});
        wallLeft.setPosition({s.wallPad, s.topPad});
        wallLeft.setFillColor(sf::Color(45, 80, 120, 160));
        window.draw(wallLeft);

        sf::RectangleShape wallRight({3.0f, s.windowH - s.topPad - 40.0f});
        wallRight.setPosition({s.playW - s.wallPad, s.topPad});
        wallRight.setFillColor(sf::Color(45, 80, 120, 160));
        window.draw(wallRight);

        sf::RectangleShape rail({s.playW - 2.0f * s.wallPad, 4.0f});
        rail.setPosition({s.wallPad, s.baseY + 18.0f});
        rail.setFillColor(sf::Color(60, 90, 130, 160));
        window.draw(rail);

        sf::RectangleShape sidebar({s.hudW, s.windowH});
        sidebar.setPosition({s.playW, 0.0f});
        sidebar.setFillColor(sf::Color(13, 20, 30));
        window.draw(sidebar);

        sf::RectangleShape divider({2.0f, s.windowH});
        divider.setPosition({s.playW - 1.0f, 0.0f});
        divider.setFillColor(sf::Color(70, 120, 180, 180));
        window.draw(divider);

        sf::RectangleShape scorePanel({s.hudW - 40.0f, 132.0f});
        scorePanel.setPosition({s.playW + 20.0f, 30.0f});
        scorePanel.setFillColor(sf::Color(18, 28, 42));
        scorePanel.setOutlineThickness(2.0f);
        scorePanel.setOutlineColor(sf::Color(70, 120, 180, 120));
        window.draw(scorePanel);

        s.font->drawTextCentered(window, "SCORE", s.playW + s.hudW * 0.5f, 52.0f, 4.5f, sf::Color(170, 215, 255));
        s.font->drawTextCentered(window, std::to_string(s.score), s.playW + s.hudW * 0.5f, 92.0f, 7.0f, sf::Color(240, 247, 255));

        sf::RectangleShape controlPanel({s.hudW - 40.0f, 188.0f});
        controlPanel.setPosition({s.playW + 20.0f, 190.0f});
        controlPanel.setFillColor(sf::Color(16, 24, 36));
        controlPanel.setOutlineThickness(2.0f);
        controlPanel.setOutlineColor(sf::Color(55, 95, 145, 110));
        window.draw(controlPanel);

        s.font->drawTextCentered(window, "CONTROL", s.playW + s.hudW * 0.5f, 206.0f, 3.6f, sf::Color(120, 185, 255));

        constexpr float infoY0 = 244.0f;
        constexpr float infoStep = 34.0f;
        constexpr float infoSize = 2.8f;

        const float labelX = s.playW + 38.0f;
        const float valueX = s.playW + 142.0f;

        s.font->drawText(window, "MOVE",  labelX, infoY0 + infoStep * 0.0f, infoSize, sf::Color(120, 185, 255));
        s.font->drawText(window, "MOUSE", valueX, infoY0 + infoStep * 0.0f, infoSize, sf::Color(230, 240, 255));
        s.font->drawText(window, "TILT",  labelX, infoY0 + infoStep * 1.0f, infoSize, sf::Color(120, 185, 255));
        s.font->drawText(window, "Q / E", valueX, infoY0 + infoStep * 1.0f, infoSize, sf::Color(230, 240, 255));
        s.font->drawText(window, "START", labelX, infoY0 + infoStep * 2.0f, infoSize, sf::Color(120, 185, 255));
        s.font->drawText(window, "SPACE", valueX, infoY0 + infoStep * 2.0f, infoSize, sf::Color(230, 240, 255));
        s.font->drawText(window, "RESET", labelX, infoY0 + infoStep * 3.0f, infoSize, sf::Color(120, 185, 255));
        s.font->drawText(window, "R",     valueX + 15.0f, infoY0 + infoStep * 3.0f, infoSize, sf::Color(230, 240, 255));

        sf::RectangleShape smilePanel({s.hudW - 40.0f, 102.0f});
        smilePanel.setPosition({s.playW + 20.0f, 386.0f});
        smilePanel.setFillColor(sf::Color(15, 24, 36));
        smilePanel.setOutlineThickness(2.0f);
        smilePanel.setOutlineColor(sf::Color(55, 95, 145, 95));
        window.draw(smilePanel);

        s.font->drawTextCentered(window, "GOOD LUCK", s.playW + s.hudW * 0.5f, 398.0f, 2.9f, sf::Color(130, 200, 255));

        if (s.restartButton) {
            s.restartButton->draw(window);
        }

        if (s.hudWidgets) {
            for (const auto& widget : *s.hudWidgets) {
                widget->draw(window);
            }
        }

        sf::Vertex arm1[] = {
            sf::Vertex({sf::Vector2f(s.base.x,  s.base.y),  sf::Color(170, 185, 210)}),
            sf::Vertex({sf::Vector2f(s.joint.x, s.joint.y), sf::Color(220, 225, 235)})
        };
        sf::Vertex arm2[] = {
            sf::Vertex({sf::Vector2f(s.joint.x, s.joint.y), sf::Color(120, 190, 255)}),
            sf::Vertex({sf::Vector2f(s.end.x,   s.end.y),   sf::Color(90, 230, 255)})
        };
        window.draw(arm1, 2, sf::PrimitiveType::Lines);
        window.draw(arm2, 2, sf::PrimitiveType::Lines);

        sf::RectangleShape baseCarriage({92.0f, 22.0f});
        baseCarriage.setOrigin({46.0f, 11.0f});
        baseCarriage.setPosition({s.base.x, s.base.y + 18.0f});
        baseCarriage.setFillColor(sf::Color(28, 42, 62));
        baseCarriage.setOutlineThickness(2.0f);
        baseCarriage.setOutlineColor(sf::Color(110, 170, 220));
        window.draw(baseCarriage);

        sf::RectangleShape racketBody({s.racketHalf * 2.0f, s.racketThickness});
        racketBody.setOrigin({s.racketHalf, s.racketThickness * 0.5f});
        racketBody.setPosition({s.end.x, s.end.y});
        racketBody.setRotation(sf::radians(s.racketTilt));
        racketBody.setFillColor(sf::Color(55, 175, 245));
        racketBody.setOutlineThickness(2.0f);
        racketBody.setOutlineColor(sf::Color(205, 245, 255));
        window.draw(racketBody);

        sf::CircleShape jointShape(8.0f);
        jointShape.setOrigin({8.0f, 8.0f});

        jointShape.setFillColor(sf::Color(255, 100, 120));
        jointShape.setPosition({s.base.x, s.base.y});
        window.draw(jointShape);

        jointShape.setFillColor(sf::Color(255, 210, 100));
        jointShape.setPosition({s.joint.x, s.joint.y});
        window.draw(jointShape);

        jointShape.setFillColor(sf::Color(110, 255, 215));
        jointShape.setPosition({s.end.x, s.end.y});
        window.draw(jointShape);

        sf::CircleShape ballGlow(s.ballRadius * 2.3f);
        ballGlow.setOrigin({s.ballRadius * 2.3f, s.ballRadius * 2.3f});
        ballGlow.setFillColor(sf::Color(70, 180, 255, 45));
        ballGlow.setPosition({s.ballPos.x, s.ballPos.y});
        window.draw(ballGlow);

        sf::CircleShape ballShape(s.ballRadius);
        ballShape.setOrigin({s.ballRadius, s.ballRadius});
        ballShape.setFillColor(sf::Color(240, 247, 255));
        ballShape.setOutlineThickness(2.0f);
        ballShape.setOutlineColor(sf::Color(120, 220, 255));
        ballShape.setPosition({s.ballPos.x, s.ballPos.y});
        ballShape.setRotation(sf::radians(s.ballVisualAngle));
        window.draw(ballShape);

        drawOverlay(window, s);
    }

private:
    static void drawOverlay(sf::RenderWindow& window, const PongRenderSnapshot& s) {
        std::visit([&]<typename T>(const T& state) {
            if constexpr (std::is_same_v<T, PlayingState>) {
            } else if constexpr (std::is_same_v<T, WaitingStartState>) {
                sf::RectangleShape overlay({s.playW, s.windowH});
                overlay.setFillColor(sf::Color(0, 0, 0, 138));
                window.draw(overlay);

                constexpr sf::Vector2f cardSize{690.0f, 278.0f};

                sf::RectangleShape banner(cardSize);
                banner.setOrigin({cardSize.x * 0.5f, cardSize.y * 0.5f});
                banner.setPosition({s.playW * 0.5f, s.windowH * 0.48f});
                banner.setFillColor(sf::Color(16, 24, 38, 238));
                banner.setOutlineThickness(3.0f);
                banner.setOutlineColor(sf::Color(110, 185, 255, 220));
                window.draw(banner);

                sf::RectangleShape accent({cardSize.x - 50.0f, 4.0f});
                accent.setOrigin({(cardSize.x - 50.0f) * 0.5f, 2.0f});
                accent.setPosition({s.playW * 0.5f, s.windowH * 0.48f - 94.0f});
                accent.setFillColor(sf::Color(80, 170, 255, 220));
                window.draw(accent);

                s.font->drawTextCentered(window, "PGA PONG", s.playW * 0.5f, s.windowH * 0.48f - 76.0f, 5.3f, sf::Color(235, 245, 255));
                s.font->drawTextCentered(window, "PRESS SPACE", s.playW * 0.5f, s.windowH * 0.48f - 4.0f, 4.3f, sf::Color(150, 210, 255));
                s.font->drawTextCentered(window, "TO START", s.playW * 0.5f, s.windowH * 0.48f + 38.0f, 3.6f, sf::Color(200, 230, 255));
                s.font->drawTextCentered(window, "MOUSE MOVE", s.playW * 0.5f, s.windowH * 0.48f + 92.0f, 2.8f, sf::Color(170, 215, 255));
                s.font->drawTextCentered(window, "Q / E TILT", s.playW * 0.5f, s.windowH * 0.48f + 118.0f, 2.8f, sf::Color(170, 215, 255));
            } else if constexpr (std::is_same_v<T, GameOverState>) {
                sf::RectangleShape overlay({s.playW, s.windowH});
                overlay.setFillColor(sf::Color(0, 0, 0, 138));
                window.draw(overlay);

                constexpr sf::Vector2f cardSize{670.0f, 236.0f};

                sf::RectangleShape banner(cardSize);
                banner.setOrigin({cardSize.x * 0.5f, cardSize.y * 0.5f});
                banner.setPosition({s.playW * 0.5f, s.windowH * 0.48f});
                banner.setFillColor(sf::Color(16, 24, 38, 238));
                banner.setOutlineThickness(3.0f);
                banner.setOutlineColor(sf::Color(110, 185, 255, 220));
                window.draw(banner);

                s.font->drawTextCentered(window, "GAME OVER", s.playW * 0.5f, s.windowH * 0.48f - 54.0f, 4.4f, sf::Color(235, 245, 255));
                s.font->drawTextCentered(window, "SCORE", s.playW * 0.5f, s.windowH * 0.48f + 4.0f, 3.4f, sf::Color(130, 200, 255));
                s.font->drawTextCentered(window, std::to_string(state.finalScore), s.playW * 0.5f, s.windowH * 0.48f + 38.0f, 4.7f, sf::Color(255, 255, 255));
                s.font->drawTextCentered(window, "PRESS R", s.playW * 0.5f, s.windowH * 0.48f + 88.0f, 3.0f, sf::Color(170, 215, 255));
                s.font->drawTextCentered(window, "TO RESTART", s.playW * 0.5f, s.windowH * 0.48f + 114.0f, 3.0f, sf::Color(170, 215, 255));
            }
        }, s.screen);
    }
};

