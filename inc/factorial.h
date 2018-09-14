#pragma once

#include <cstdint>

constexpr uint32_t factorial(uint32_t n) {
  return n == 0 ? 1 : factorial(n - 1) * n;
}