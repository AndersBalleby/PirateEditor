#pragma once

#include <cmath>
#include <ostream>
struct Vec2 {
  float x = 0.0f;
  float y = 0.0f;

  constexpr Vec2() = default;
  constexpr Vec2(float x, float y) : x(x), y(y) {}
  
  // Kan tilføje flere operator overloads senere hvis der er behov for det
  constexpr Vec2 operator+(const Vec2 &rhs) const { return {x + rhs.x, y + rhs.y}; }
  constexpr Vec2 operator-(const Vec2 &rhs) const { return {x - rhs.x, y - rhs.y}; }
  constexpr bool operator==(const Vec2 &rhs) const { return x == rhs.x && y == rhs.y; }

  float length() const { return std::sqrt(x*x + y*y); }
};

// til debug
inline std::ostream& operator<<(std::ostream& os, const Vec2 &v) {
  return os << "(" << v.x << ", " << v.y << ")";
}


