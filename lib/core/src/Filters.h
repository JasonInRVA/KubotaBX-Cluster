#pragma once

// Tiny one-pole low-pass helper (for later)
template<typename T>
class LowPass {
public:
  explicit LowPass(float alpha = 0.2f) : a(alpha), y{} {}
  T update(T x) { y = (T)(a * x + (1.0f - a) * y); return y; }
  T value() const { return y; }
private:
  float a;
  T y;
};
