#include <gtest/gtest.h>

#include "game3/PingPongGame.hpp"

#include <variant>

namespace {
    constexpr float EPS = 1e-3f;
}

TEST(BallEntity, LaunchInitializesPositionAndVelocity) {
    BallEntity ball;
    ball.launch(50.0f, 60.0f);

    EXPECT_NEAR(ball.pos.x, 50.0f, EPS);
    EXPECT_NEAR(ball.pos.y, 60.0f, EPS);
    EXPECT_NEAR(ball.prevPos.x, 50.0f, EPS);
    EXPECT_NEAR(ball.prevPos.y, 60.0f, EPS);
    EXPECT_NEAR(ball.vel.x, 5.6f, EPS);
    EXPECT_NEAR(ball.vel.y, 6.1f, EPS);
}

TEST(BallEntity, ParkStopsBallAndKeepsRequestedPosition) {
    BallEntity ball;
    ball.park(10.0f, 20.0f);

    EXPECT_NEAR(ball.pos.x, 10.0f, EPS);
    EXPECT_NEAR(ball.pos.y, 20.0f, EPS);
    EXPECT_NEAR(ball.prevPos.x, 10.0f, EPS);
    EXPECT_NEAR(ball.prevPos.y, 20.0f, EPS);
    EXPECT_NEAR(ball.vel.x, 0.0f, EPS);
    EXPECT_NEAR(ball.vel.y, 0.0f, EPS);
}

TEST(BallEntity, StepMovesBallAndUpdatesPreviousPosition) {
    BallEntity ball;
    ball.launch(0.0f, 0.0f);

    const float oldX = ball.pos.x;
    const float oldY = ball.pos.y;

    ball.step(1.0f);

    EXPECT_NEAR(ball.prevPos.x, oldX, EPS);
    EXPECT_NEAR(ball.prevPos.y, oldY, EPS);
    EXPECT_NEAR(ball.pos.x, oldX + 5.6f, EPS);
    EXPECT_NEAR(ball.pos.y, oldY + 6.1f, EPS);
}

TEST(PingPongGame, StartsWithZeroResult) {
    PingPongGame game;

    ASSERT_TRUE(std::holds_alternative<int>(game.getResult()));
    EXPECT_EQ(std::get<int>(game.getResult()), 0);
}
