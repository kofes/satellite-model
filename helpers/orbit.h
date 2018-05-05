#pragma once

#include <tuple>
#include <cmath>
#include <LinearAlgebra.h>
#include "containers.h"
#include "constants.h"

namespace helper {
namespace orbit {
    double anomaly(double dt, double p, double e, double mu);

    std::tuple<double, double> ellipticPolarCoordinates(double dt, double p, double e, double mu);

    double shadow(
            const helper::container::KeplerParameters& keplerParameters,
            const linear_algebra::Vector& solar_norm,
            const helper::container::SailParameters& sailParameters
    );
}
}