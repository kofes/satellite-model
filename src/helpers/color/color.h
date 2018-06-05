#pragma once

#include <cstdint>
#include <tuple>

namespace helper {
struct color {
    explicit color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0);

    uint8_t& operator[](size_t index);
    uint8_t operator[](size_t index) const;

    std::tuple<double, double, double> d() const;
    std::tuple<float, float, float> f() const;

    double d(size_t index) const;
    float f(size_t index) const;

    uint8_t r, g, b;
};
}   // namespace helper
