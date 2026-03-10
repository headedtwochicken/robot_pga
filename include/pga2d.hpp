#pragma once
 #include <cmath>
 // PGA 2D: базис 1, e0, e1, e2, e01, e20, e12, e012
 struct Multivector {
     float s;      // scalar
     float e1, e2; // векторы компоненты по осям x и y
     float e0;     // origin
     float e12;    // bivector (rotation) вращение
     float e01, e20; // bivectors (translation) сдвиг

     Multivector() : s(0), e1(0), e2(0), e0(0), e12(0), e01(0), e20(0) {}
 };
inline Multivector operator*(const Multivector& a, const Multivector& b) {
    Multivector r;

    // 1. Скалярная часть и вращение (работает как умножение комплексных чисел)
    r.s   = a.s * b.s - a.e12 * b.e12;
    r.e12 = a.s * b.e12 + a.e12 * b.s;

    // 2. Сдвиги
    r.e01 = a.s * b.e01 + a.e01 * b.s - a.e12 * b.e20 + a.e20 * b.e12;
    r.e20 = a.s * b.e20 + a.e20 * b.s + a.e12 * b.e01 - a.e01 * b.e12;

    return r;
}

// Reverse: ~(a + b) = a - b для bivector
inline Multivector reverse(const Multivector& m) {
    Multivector r = m;
    r.e12 = -r.e12;
    r.e01 = -r.e01;
    r.e20 = -r.e20;
    return r;
}

inline Multivector makeRotor(float theta) {
    Multivector r;
    float half = theta * 0.5f;
    r.s = std::cos(half);
    r.e12 = std::sin(half);
    return r;
}

inline Multivector makeTranslator(float dx, float dy) {
    Multivector t;
    t.s = 1.0f;
    t.e01 = dx * 0.5f;
    t.e20 = dy * 0.5f;
    return t;
}

inline Multivector makePoint(float x, float y) {
    Multivector p;
    p.e1 = x;
    p.e2 = y;
    p.e0 = 1.0f;
    return p;
}

inline void getPoint(const Multivector& p, float& x, float& y) {
    if (p.e0 != 0) {
        x = p.e1 / p.e0;
        y = p.e2 / p.e0;
    } else {
        x = p.e1; y = p.e2;
    }
}
inline Multivector applyRotor(const Multivector& r, const Multivector& p) {
    [[maybe_unused]] Multivector r_rev = reverse(r);
    Multivector result;
    float c = r.s * r.s - r.e12 * r.e12;
    float s = 2 * r.s * r.e12;

    result.e1 = c * p.e1 - s * p.e2;
    result.e2 = s * p.e1 + c * p.e2;
    result.e0 = p.e0;

    return result;
}

inline Multivector applyMotor(const Multivector& m, const Multivector& p) {
    Multivector result;

    float c = m.s * m.s - m.e12 * m.e12;
    float s = 2.0f * m.s * m.e12;

    float rx = c * p.e1 - s * p.e2;
    float ry = s * p.e1 + c * p.e2;

    float tx = 2.0f * (m.s * m.e01 - m.e12 * m.e20);
    float ty = 2.0f * (m.s * m.e20 + m.e12 * m.e01);

    result.e1 = rx + tx * p.e0;
    result.e2 = ry + ty * p.e0;
    result.e0 = p.e0;

    return result;
}
inline Multivector operator+(const Multivector& a, const Multivector& b) {
    Multivector r;
    r.s   = a.s + b.s;
    r.e1  = a.e1 + b.e1;
    r.e2  = a.e2 + b.e2;
    r.e0  = a.e0 + b.e0;
    r.e12 = a.e12 + b.e12;
    r.e01 = a.e01 + b.e01;
    r.e20 = a.e20 + b.e20;
    return r;
}

// Умножение мультивектора на обычное число (скаляр)
inline Multivector operator*(const Multivector& m, float f) {
    Multivector r;
    r.s   = m.s * f;
    r.e1  = m.e1 * f;
    r.e2  = m.e2 * f;
    r.e0  = m.e0 * f;
    r.e12 = m.e12 * f;
    r.e01 = m.e01 * f;
    r.e20 = m.e20 * f;
    return r;
}
inline Multivector normalizeMotor(const Multivector& m) {
    float norm = std::sqrt(m.s * m.s + m.e12 * m.e12);
    if (norm == 0.0f) return m;
    return m * (1.0f / norm);
}

inline Multivector makeVelocity(float w, float vx, float vy) {
    Multivector v;
    v.e12 = w;
    v.e01 = vx;
    v.e20 = vy;
    return v;
}