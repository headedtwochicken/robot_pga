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