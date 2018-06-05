#pragma once

#include <math_models/KeplerDifferentials/KeplerDifferentials.h>

namespace force {
linear_algebra::Vector gravJ2(const helper::container::KeplerParameters& params,
    const double mass, const double mu, const double r, const double nu);

linear_algebra::Vector atmos(const helper::container::KeplerParameters& params,
                                    double rho_atm,
                                    double sail_area,
                                    const linear_algebra::Vector& sail_norm,
                                    const double mu, const double r, const double nu
);

linear_algebra::Vector solar(const helper::container::SailParameters& params, const linear_algebra::Vector& solar_norm);

double atm_density(double h, double L = -0.0065);
}
