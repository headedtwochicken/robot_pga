#include <gtest/gtest.h>

#include "include/game3/PingPongGameView.hpp"

#include <memory>
#include <variant>

TEST(PingPongViewSupport, PongVec2ArithmeticWorksCorrectly) {
    PongVec2 a{2.0f, 3.0f};
    PongVec2 b{4.0f, -1.0f};

    const PongVec2 sum = a + b;
    const PongVec2 diff = a - b;
    const PongVec2 mul = a * 2.0f;
    const PongVec2 div = b / 2.0f;

    EXPECT_FLOAT_EQ(sum.x, 6.0f);
    EXPECT_FLOAT_EQ(sum.y, 2.0f);

    EXPECT_FLOAT_EQ(diff.x, -2.0f);
    EXPECT_FLOAT_EQ(diff.y, 4.0f);

    EXPECT_FLOAT_EQ(mul.x, 4.0f);
    EXPECT_FLOAT_EQ(mul.y, 6.0f);

    EXPECT_FLOAT_EQ(div.x, 2.0f);
    EXPECT_FLOAT_EQ(div.y, -0.5f);
}

TEST(PingPongViewSupport, ScreenStateStoresAllExpectedAlternatives) {
    ScreenState waiting = WaitingStartState{};
    ScreenState playing = PlayingState{};
    ScreenState gameOver = GameOverState{12};

    EXPECT_TRUE(std::holds_alternative<WaitingStartState>(waiting));
    EXPECT_TRUE(std::holds_alternative<PlayingState>(playing));

    ASSERT_TRUE(std::holds_alternative<GameOverState>(gameOver));
    EXPECT_EQ(std::get<GameOverState>(gameOver).finalScore, 12);
}

TEST(PingPongViewSupport, PixelBitmapFontWidthIsConsistent) {
    PixelBitmapFont font;

    EXPECT_FLOAT_EQ(font.textWidth("", 4.0f), 0.0f);
    EXPECT_GT(font.textWidth("A", 4.0f), 0.0f);
    EXPECT_GT(font.textWidth("AAA", 4.0f), font.textWidth("A", 4.0f));
}

TEST(PingPongViewSupport, SmileyWidgetRejectsNullFont) {
    EXPECT_THROW(
        SmileyWidget(10.0f, 10.0f, 5.0f, nullptr),
        std::invalid_argument
    );
}

TEST(PingPongViewSupport, RestartButtonRejectsNullFont) {
    EXPECT_THROW(
        RestartButtonWidget(sf::FloatRect({0.0f, 0.0f}, {100.0f, 40.0f}), nullptr),
        std::invalid_argument
    );
}

TEST(PingPongViewSupport, RestartButtonContainsInsideAndOutsidePoints) {
    auto font = std::make_shared<PixelBitmapFont>();
    RestartButtonWidget button(sf::FloatRect({20.0f, 30.0f}, {100.0f, 40.0f}), font);

    EXPECT_TRUE(button.contains({25.0f, 35.0f}));
    EXPECT_TRUE(button.contains({119.0f, 69.0f}));
    EXPECT_FALSE(button.contains({10.0f, 10.0f}));
    EXPECT_FALSE(button.contains({200.0f, 200.0f}));
}

TEST(PingPongViewSupport, SnapshotStoresPointersAndState) {
    auto font = std::make_shared<PixelBitmapFont>();
    RestartButtonWidget button(sf::FloatRect({0.0f, 0.0f}, {100.0f, 40.0f}), font);

    std::vector<std::unique_ptr<UiWidget>> widgets;
    widgets.push_back(std::make_unique<SmileyWidget>(10.0f, 20.0f, 5.0f, font));

    PongRenderSnapshot snapshot;
    snapshot.font = font;
    snapshot.restartButton = &button;
    snapshot.hudWidgets = &widgets;
    snapshot.score = 7;
    snapshot.screen = GameOverState{7};

    ASSERT_TRUE(snapshot.font != nullptr);
    ASSERT_TRUE(snapshot.restartButton != nullptr);
    ASSERT_TRUE(snapshot.hudWidgets != nullptr);
    EXPECT_EQ(snapshot.score, 7);

    ASSERT_TRUE(std::holds_alternative<GameOverState>(snapshot.screen));
    EXPECT_EQ(std::get<GameOverState>(snapshot.screen).finalScore, 7);
}
