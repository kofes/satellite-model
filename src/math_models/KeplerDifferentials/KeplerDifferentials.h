#pragma once

#include <LinearAlgebra.h>
#include <helpers/helpers.h>

namespace kepler {
namespace differentials {
    typedef double (*Integral)(helper::integral::function fun, double from, double to, double dh);

    helper::container::KeplerParameters parameters(
            const helper::container::KeplerParameters& params,
            const double mainMass,
            const double satMass,
            const double t,
            const linear_algebra::Vector &force);

    double dpdt(double r, double mu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double dedt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double domegadt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double didt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double dOmegadt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double dtaudt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f, Integral integ = helper::integral::rect);

    double dexdt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double deydt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double dudt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
    double r(double p, double e, double omega, double u);
    double r(double p, double e, double nu);
}   // namespace differentials
}   // namespace kepler
