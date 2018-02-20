#pragma once

#include <cstdint>
#include <tuple>

namespace helper {
struct color {
    color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    inline std::tuple<double, double, double> d() {
        return std::make_tuple(r / 255., g / 255., b / 255.);
    };

    inline std::tuple<float, float, float> f() {
        return std::make_tuple(r / 255.f, g / 255.f, b / 255.f);
    };

    uint8_t r, g, b;
};
}
