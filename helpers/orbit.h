#pragma once

#include <tuple>
#include <cmath>

namespace helper {
namespace orbit {
    double anomaly(double dt, double p, double e, double mu);

    std::tuple<double, double> ellipticPolarCoordinates(double dt, double p, double e, double mu);
}
}