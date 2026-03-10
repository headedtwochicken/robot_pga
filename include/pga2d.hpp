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