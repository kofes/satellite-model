#include "color.h"

namespace helper {
color::color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

uint8_t& color::operator[](size_t index) {
    switch (index) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        default: throw(std::runtime_error("[helper:color] error: index > 2"));
    }
}

uint8_t color::operator[](size_t index) const {
    switch (index) {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        default: throw(std::runtime_error("[helper:color] error: index > 2"));
    }
}

std::tuple<double, double, double> color::d() const {
    return std::make_tuple(r / 255., g / 255., b / 255.);
}

std::tuple<float, float, float> color::f() const {
    return std::make_tuple(r / 255.f, g / 255.f, b / 255.f);
}

double color::d(size_t index) const {
    switch (index) {
        case 0: return r / 255.;
        case 1: return g / 255.;
        case 2: return b / 255.;
        default: throw(std::runtime_error("[helper:color] error: index > 2"));
    }
}

float color::f(size_t index) const {
    switch (index) {
        case 0: return r / 255.f;
        case 1: return g / 255.f;
        case 2: return b / 255.f;
        default: throw(std::runtime_error("[helper:color] error: index > 2"));
    }
}
}   // namespace helper
