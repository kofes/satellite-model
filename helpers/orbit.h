#pragma once

#include <tuple>
#include <cmath>
#include <LinearAlgebra.h>
#include <objects/objects.h>
#include <mvp/actions/actions.h>
#include "containers.h"
#include "constants.h"

namespace helper {
namespace orbit {
    double E(
            const helper::container::KeplerParameters& kepParams,
            const phys::object& main,
            const phys::object& sat,
            double t, double epsilon = 1e-5
    );

    double nu(const helper::container::KeplerParameters& kepParams, double E);

    double r(const helper::container::KeplerParameters& kepParams, double E);

    linear_algebra::Vector r(double r, double nu);

    linear_algebra::Vector r(
            const helper::container::KeplerParameters& kepParams,
            const linear_algebra::Vector& r
    );

    double T(
            const helper::container::KeplerParameters& kepParams,
            const phys::object& main,
            const phys::object& sat
    );
}
}