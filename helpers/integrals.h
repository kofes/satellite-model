#pragma once

#include <cstdlib>

namespace integral {
    typedef double (*dfdt)(double dt);

    inline double rect(dfdt f, double from, double to, size_t N) {
        double sum = 0;
        double dx = (to - from)/N;
        for (size_t i = 0; i < N; ++i)
            sum += f(from + dx*i) * dx;
        return sum;
    }

    inline double trapez(dfdt f, double from, double to, size_t N) {
        double sum = 0;
        double dx = (to - from)/N;
        for (size_t i = 0; i < N-1; ++i)
            sum += (f(from + dx * i) + f(from + dx * (i+1))) * dx / 2;
        return sum;
    }

    inline double simpson(dfdt f, double from, double to, size_t N) {
        double sum = 0;
        double dx = (to - from)/N;
        sum += f(from);
        for (size_t i = 0; i < N; ++i)
            sum += 4 * f(from + dx * (2*i+1) / 2);
        for (size_t i = 1; i < N; ++i)
            sum += 2 * f(from + dx * i);
        sum += f(to);
        sum *= dx / 6;
        return sum;
    }

};