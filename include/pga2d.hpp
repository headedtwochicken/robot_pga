#pragma once //защита от повторного включения хед файла (Если pga2d.hpp где-то подключится дважды через цепочку #include, компилятор не будет определять всё второй раз.)
 #include <cmath> //для кос син и sqrt
 // PGA 2D: базис 1(скаляр), e0, e1, e2,(базисные направления) e01, e20, e12,(бивекторы)
 struct Multivector {
     // как бы мы так храним: м= s*1 +e1*E1+ e2*E2 + E0*e0 + e12* E12 + e01 * E01 * e20* E20
     // E - числовые поля
     float s;      // scalar
     float e1, e2; // векторы компоненты по осям x и y (для представления точки)
     float e0;     // origin если e0 существует то это точка, но если e0 не сущ то у него нет привязки к координатам, то есть это вектор на бесконечности
     // следовательно точка хранится как (x,y,1)
     float e12;    // bivector (rotation) вращение ( ориентация в плоскости x-y)
     float e01,/*перенос по x*/ e20;/*перенос по y*/ // bivectors (translation) сдвиг

     Multivector() : s(0), e1(0), e2(0), e0(0), e12(0), e01(0), e20(0) {} //констурктор по умолчанию создается нулевой мальтивектор
 };
inline Multivector operator*(const Multivector& a, const Multivector& b) { // урезанная формула, работает для скаляров, вращения и переноса
    Multivector r;

    // 1. Скалярная часть и вращение (работает как умножение комплексных чисел)
    r.s   = a.s * b.s - a.e12 * b.e12; //s - реальная часть  e12-мнимая часть( важно потому что повороты через синусы и косинусы)
    r.e12 = a.s * b.e12 + a.e12 * b.s;

    // 2. Сдвиги
    r.e01 = a.s * b.e01 + a.e01 * b.s - a.e12 * b.e20 + a.e20 * b.e12; //тут комбинируются  поворот пренос и еще один поворот или перенос именно поэтому мы еще учитывает e12
    r.e20 = a.s * b.e20 + a.e20 * b.s + a.e12 * b.e01 - a.e01 * b.e12;
// если обьект уже повернут то его переносы по локальным осям не совпадают с глобальными поэтому он сначала делает переориентировку а после нормальным перенос
    return r; // результат в виде нового мультивектора как комбинация двух
}

// Reverse: ~(a + b) = a - b для bivector
inline Multivector reverse(const Multivector& m) {
    Multivector r = m;
    r.e12 = -r.e12;
    r.e01 = -r.e01;
    r.e20 = -r.e20;
    return r;
}

inline Multivector makeRotor(float theta/*угол поворота в радианах*/) { // создание поворота
    Multivector r;
    float half = theta * 0.5f; //в геом алгебре ротор через половинный угол
    r.s = std::cos(half);//скалярная часть
    r.e12 = std::sin(half);// бивекторная часть
    // только скаляр и e12 потому что поворот не требует всего остального, чисто вращение
    return r;
}

inline Multivector makeTranslator(float dx/*на сколько сдвинуть по х*/, float dy/*на сколько сдвинуть по y*/) {// создание переноса
    Multivector t;
    t.s = 1.0f; // 1 потому что единичное преобразование
    t.e01 = dx * 0.5f; // также через половинные углы только при создании потом угол умножается на 2 потому что такая формула
    t.e20 = dy * 0.5f;
    return t;
}

inline Multivector makePoint(float x, float y) { // создание точки вида (x,y,1)
    // так удобнее чтобы было единообразие сразу и для трансформ
    Multivector p;
    p.e1 = x;
    p.e2 = y;
    p.e0 = 1.0f;
    return p;
}

inline void getPoint(const Multivector& p, float& x, float& y) { // чтобы получить обычные координаты x y
    if (p.e0 != 0) { // деление на вес
        x = p.e1 / p.e0;
        y = p.e2 / p.e0;
    } else { // если уже обычная точка
        x = p.e1; y = p.e2;
    }
}
inline Multivector applyRotor(const Multivector& r, const Multivector& p) {
    Multivector result;
    //r - ротор который хранит поворот p-точка
    float c = r.s * r.s - r.e12 * r.e12; // формула двойного косинуса как кос в кв - син в кв = обычный кос. чтобы из половинного получился обычный
    float s = 2 * r.s * r.e12; // формула двойного синуса, суть та же что и с косинусом
//формула поворота точки через матрицы
    result.e1 = c * p.e1 - s * p.e2;
    result.e2 = s * p.e1 + c * p.e2;
    result.e0 = p.e0; // чтобы не менялся тип точки

    return result;
}

inline Multivector applyMotor(const Multivector& m, const Multivector& p) {
    Multivector result;
//вычисление вращения (m (мотор) но также содержит роторную часть
    float c = m.s * m.s - m.e12 * m.e12; // кос двойного
    float s = 2.0f * m.s * m.e12;//син двойного
// поворот
    float rx = c * p.e1 - s * p.e2; //rx - координата x после поворота
    float ry = s * p.e1 + c * p.e2; //ry - координата y после поворота
// вычисление переноса из компонентов мотора
    float tx = 2.0f * (m.s * m.e01 - m.e12 * m.e20);
    float ty = 2.0f * (m.s * m.e20 + m.e12 * m.e01);
//применение переноса
    result.e1 = rx + tx * p.e0; // если e0 = 1 то перенос должен срабоать
    result.e2 = ry + ty * p.e0;// если 1 то перенос не влияет на направление
    result.e0 = p.e0;// сохраняется геометрический тип

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
inline Multivector normalizeMotor(const Multivector& m) {//нормировка мотора по его роторной части(к "правильной длине" по вращательной части)
    // нормируем потому что иначе функции applyMotor and applyRotor будут не корректно работать
    float norm = std::sqrt(m.s * m.s + m.e12 * m.e12); // s and e12 задают вращательную/роторную часть
    // вычисление длины
    if (norm == 0.0f) return m;// если длина ноль то на нее нельяз делить
    return m * (1.0f / norm); // если длина не ноль то умножаем на обратное число, приводим к единице
}

inline Multivector makeVelocity(float w, float vx, float vy) {// w - угловая скорость vx, vy линейная скорость по x and y ( динамика)
    Multivector v;
    v.e12 = w;
    v.e01 = vx;
    v.e20 = vy;
    return v;
}