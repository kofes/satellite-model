#pragma once

#include <LinearAlgebra.h>

namespace helper {
    namespace container {
        struct SailParameters {
            double Bf = 0, Bb = 0;
            double ef = 0, eb = 0;
            double rho = 0, s = 0;
            double area = 0;
            linear_algebra::Vector norm = {1, 0, 0, 1};
        };

        struct KeplerParameters {
            double e = 1;
            double p = 1;
            double omega = 0;
            double Omega = 0;
            double i = 0;
            double tau = 0;
        };
    }
}