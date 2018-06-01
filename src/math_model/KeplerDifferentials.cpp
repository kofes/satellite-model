#include "KeplerDifferentials.h"

namespace kepler {
linear_algebra::Vector Differentials::operator()(
        const helper::container::KeplerParameters& params,
        const phys::object& main,
        const phys::object& sat,
        const double t,
        const linear_algebra::Vector &force
) {
    double mu = helper::constant::G * main.mass();
    double E = helper::orbit::E(params, main, sat, t);
    double r = helper::orbit::r(params, E);
    double nu = helper::orbit::nu(params, E);

    return linear_algebra::Vector {
        dpdt(r, mu, params, force),
        dedt(r, mu, nu, params, force),
        domegadt(r, mu, nu, params, force),
        dOmegadt(r, mu, nu, params, force),
        didt(r, mu, nu, params, force),
        dtaudt(r, mu, nu, params, force)
    };
}

inline double Differentials::dpdt(double r, double mu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    return 2 * r * std::sqrt(params.p / mu) * f[1];
}

inline double Differentials::dedt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    return std::sqrt(params.p / mu) *
           (f[0] * std::sin(nu) + f[1] * ((1 + r / params.p) * std::cos(nu) + params.e * r / params.p));
}

inline double Differentials::domegadt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    const double u = params.omega + nu;
    return 1 / params.e * std::sqrt(params.p / mu) * (
            - f[0] * std::cos(nu)
            + f[1] * (1 + r / params.p) * std::sin(nu)
            - f[2] * params.e * r / params.p * std::sin(u) / std::tan(params.i)
    );
}

inline double Differentials::didt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    const double u = params.omega + nu;
    return r / std::sqrt(mu * params.p) * std::cos(u) * f[2];
}

inline double Differentials::dOmegadt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    const double u = params.omega + nu;
    return r / std::sqrt(mu * params.p) * std::sin(u) / std::sin(params.i) * f[2];
}

inline double Differentials::dtaudt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f, Integral integ = helper::integral::rect) {
    auto fun = [&](double dnu) -> double {
        return 2 * std::cos(dnu) / std::pow(1 + params.e / std::cos(dnu), 3);
    };
    double N = std::pow(params.p / r, 2) * integ(fun, 0, nu, 1000);
    return r * r / (params.e * mu) * (
            (params.e * N * std::sin(nu) - std::cos(nu)) * f[0] +
            N * params.p / r * f[1]
    );
}

inline double Differentials::dexdt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    const double ex =  params.e * std::cos(params.omega);
    const double ey =  params.e * std::sin(params.omega);
    const double u = params.omega + nu;

    return std::sqrt(params.p / mu) * (
            f[0] * std::sin(u)
            +f[1] * ((1 + r / params.p)*std::cos(u) + r * ex / params.p)
            +f[2] * r * ey / params.p * std::cos(params.i) * std::sin(u)
    );
}

inline double Differentials::deydt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    const double ex =  params.e * std::cos(params.omega);
    const double ey =  params.e * std::sin(params.omega);
    const double u = params.omega + nu;

    return std::sqrt(params.p / mu) * (
            -f[0] * std::cos(u)
            +f[1] * ((1 + r / params.p)*std::sin(u) + r * ey / params.p)
            -f[2] * r * ex / params.p * std::cos(params.i) * std::sin(u)
    );
}

inline double Differentials::dudt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f) {
    const double u = params.omega + nu;

    return std::sqrt(mu * params.p) / (r * r)
           - f[2] * r * std::cos(params.i) * std::sin(u)
             / std::sqrt(mu * params.p);
}

inline double Differentials::r(double p, double e, double omega, double u) {
    double ex =  e * std::cos(omega);
    double ey =  e * std::sin(omega);
    return p / (1 + ex * std::cos(u) + ey * std::sin(u));
}

inline double Differentials::r(double p, double e, double nu) {
    return p / (1 + e * std::cos(nu));
}
}