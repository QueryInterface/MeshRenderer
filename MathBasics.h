#pragma once

template <class T>
struct Vector1 {
public:
    Vector1() {}
    Vector1(T x_) : x(x_) {}
    T x;
};

template <class T>
struct Vector2 {
    Vector2() {}
    Vector2(T x_, T y_) : x(x_), y(y_) {}
    T x;
    T y;
};

template <class T>
struct Vector3 {
    Vector3() {}
    Vector3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
    T x;
    T y;
    T z;
};

template <class T>
struct Vector4 {
    Vector4() {}
    Vector4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
    T x;
    T y;
    T z;
    T w;
};
