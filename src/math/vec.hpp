#pragma once
#include <format>
#include <iostream>
#include <string>
#include <cmath>

/* Meget simpel 2d Vector class */
template <typename T>
class Vec2 {
  public:
    T x, y;

    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}

  constexpr Vec2 operator+(const Vec2& other) const {
    return Vec2(other.x + x, other.y + y);
  }

  constexpr Vec2 operator-(const Vec2& other) const {
    return Vec2(x - other.x, y - other.y);
  }

  constexpr Vec2& operator+=(const Vec2& other) {
    x += other.x; y += other.y;
    return *this;
  }

  constexpr Vec2& operator-=(const Vec2& other) {
    x -= other.x; y -= other.y;
    return *this;
  }

  constexpr bool operator==(const Vec2& other) {
    return (x == other.x && y == other.y);
  }

  /* Returnerer en skalar */
  constexpr T operator*(const Vec2& other) const {
    return x * other.x + y * other.y;
  }

  constexpr T length() const {
    return std::sqrt(x * x + y * y);
  }

  constexpr T distance(const Vec2& other) const {
    T dx = other.x - x;
    T dy = other.y - y;
    return std::sqrt(dx*dx + dy*dy);
  }

  friend std::ostream& operator<<(std::ostream& os, const Vec2<T>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
  }
};
