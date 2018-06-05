#include "integrals.h"

namespace helper {
namespace integral {
double rect(function f, double from, double to, double dh) {
    double sum = 0;
    double acc = 0;

    for (size_t i = 0; i < std::fabs(to - from) / dh; ++i) {
        double y = f(from + dh * i) * dh - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }

    return sum;
}

double trapez(function f, double from, double to, double dh) {
    double sum = 0;
    double acc = 0;

    for (size_t i = 0; i < std::fabs(to - from)/dh - 1; ++i) {
        double y = (f(from + dh * i) + f(from + dh * (i + 1))) * dh / 2 - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }

    return sum;
}

double simpson(function f, double from, double to, double dh) {
    double sum = 0;
    double acc = 0;

    sum += f(from);

    for (size_t i = 0; i < std::fabs(to - from)/dh; ++i) {
        double y = 4 * f(from + dh * (2 * i + 1) / 2) - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }
    for (size_t i = 1; i < std::fabs(to - from)/dh; ++i) {
        double y = 2 * f(from + dh * i) - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }

    sum += f(to);

    sum *= dh / 6;

    return sum;
}
}   // namespace integral
}   // namespace helper
