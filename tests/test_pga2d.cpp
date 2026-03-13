#include <gtest/gtest.h>

#include "include/pga2d.hpp"

#include <cmath>

namespace {
constexpr float EPS = 1e-3f;
constexpr float PI = 3.14159265358979323846f;

void ExpectPointNear(const Multivector& p, float expectedX, float expectedY, float eps = EPS) {
    float x = 0.0f;
    float y = 0.0f;
    getPoint(p, x, y);
    EXPECT_NEAR(x, expectedX, eps);
    EXPECT_NEAR(y, expectedY, eps);
}
}

TEST(PGA2D, MakePointAndGetPointRoundTrip) {
    const Multivector p = makePoint(3.5f, -2.25f);
    ExpectPointNear(p, 3.5f, -2.25f);
}

TEST(PGA2D, TranslatorMovesOriginToExpectedPosition) {
    const Multivector origin = makePoint(0.0f, 0.0f);
    const Multivector t = makeTranslator(15.0f, -25.0f);
    const Multivector result = applyMotor(t, origin);

    ExpectPointNear(result, 15.0f, -25.0f);
}

TEST(PGA2D, RotorRotatesPointByNinetyDegrees) {
    const Multivector p = makePoint(10.0f, 0.0f);
    const Multivector r = makeRotor(PI / 2.0f);

    const Multivector result = applyMotor(r, p);
    ExpectPointNear(result, 0.0f, 10.0f);
}

TEST(PGA2D, ApplyRotorMatchesApplyMotorForPureRotation) {
    const Multivector p = makePoint(4.0f, 2.0f);
    const Multivector r = makeRotor(PI / 3.0f);

    const Multivector rotatedOnly = applyRotor(r, p);
    const Multivector motorVersion = applyMotor(r, p);

    float x1 = 0.0f, y1 = 0.0f;
    float x2 = 0.0f, y2 = 0.0f;
    getPoint(rotatedOnly, x1, y1);
    getPoint(motorVersion, x2, y2);

    EXPECT_NEAR(x1, x2, EPS);
    EXPECT_NEAR(y1, y2, EPS);
}

TEST(PGA2D, ComposedTranslatorsAddOffsets) {
    const Multivector p = makePoint(0.0f, 0.0f);
    const Multivector t1 = makeTranslator(5.0f, 7.0f);
    const Multivector t2 = makeTranslator(-2.0f, 3.0f);

    const Multivector combined = t1 * t2;
    const Multivector result = applyMotor(combined, p);

    ExpectPointNear(result, 3.0f, 10.0f);
}

TEST(PGA2D, CombinedMotorMatchesSequentialApplication) {
    const Multivector p = makePoint(1.0f, 0.0f);
    const Multivector r = makeRotor(PI / 2.0f);
    const Multivector t = makeTranslator(10.0f, -2.0f);

    const Multivector sequential = applyMotor(t, applyMotor(r, p));
    const Multivector combined = applyMotor(t * r, p);

    float xs = 0.0f, ys = 0.0f;
    float xc = 0.0f, yc = 0.0f;
    getPoint(sequential, xs, ys);
    getPoint(combined, xc, yc);

    EXPECT_NEAR(xs, xc, EPS);
    EXPECT_NEAR(ys, yc, EPS);
}

TEST(PGA2D, ReverseNegatesBivectorPartsOnly) {
    Multivector m;
    m.s = 2.0f;
    m.e12 = 3.0f;
    m.e01 = -4.0f;
    m.e20 = 5.0f;

    const Multivector r = reverse(m);

    EXPECT_FLOAT_EQ(r.s, 2.0f);
    EXPECT_FLOAT_EQ(r.e12, -3.0f);
    EXPECT_FLOAT_EQ(r.e01, 4.0f);
    EXPECT_FLOAT_EQ(r.e20, -5.0f);
}

TEST(PGA2D, NormalizeMotorPreservesTransformation) {
    const Multivector original = makeRotor(PI / 4.0f);
    const Multivector scaled = original * 5.0f;
    const Multivector normalized = normalizeMotor(scaled);

    const Multivector p = makePoint(2.0f, 1.0f);

    const Multivector a = applyMotor(original, p);
    const Multivector b = applyMotor(normalized, p);

    float ax = 0.0f, ay = 0.0f;
    float bx = 0.0f, by = 0.0f;
    getPoint(a, ax, ay);
    getPoint(b, bx, by);

    EXPECT_NEAR(ax, bx, EPS);
    EXPECT_NEAR(ay, by, EPS);
}

TEST(PGA2D, MakeVelocityStoresAngularAndLinearParts) {
    const Multivector v = makeVelocity(1.5f, -2.0f, 3.25f);

    EXPECT_FLOAT_EQ(v.e12, 1.5f);
    EXPECT_FLOAT_EQ(v.e01, -2.0f);
    EXPECT_FLOAT_EQ(v.e20, 3.25f);
}
