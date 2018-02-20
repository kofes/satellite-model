#pragma once

#include <cstdint>
#include <tuple>

namespace helper {
struct color {
    color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    inline uint8_t& operator[](uint8_t index) {
        switch (index) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            default: throw(std::runtime_error("[helper:color] error: index > 2"));
        }
    }

    inline uint8_t operator[](uint8_t index) const {
        switch (index) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            default: throw(std::runtime_error("[helper:color] error: index > 2"));
        }
    }

    inline std::tuple<double, double, double> d() const {
        return std::make_tuple(r / 255., g / 255., b / 255.);
    };

    inline std::tuple<float, float, float> f() const {
        return std::make_tuple(r / 255.f, g / 255.f, b / 255.f);
    };

    uint8_t r, g, b;
};
}