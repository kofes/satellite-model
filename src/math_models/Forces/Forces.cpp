#include "Forces.h"

namespace force {
linear_algebra::Vector gravJ2(const helper::container::KeplerParameters& params,
    const double mass, const double mu, const double r, const double nu) {
    double u = params.omega + nu;
    return 3 * helper::constant::J2 * mu * std::pow(helper::constant::EARTH_R, 2) / (2 * std::pow(r, 4)) * mass *
            linear_algebra::Vector {
        3 * std::pow(std::sin(params.i), 2) * std::pow(std::sin(u), 2) - 1,
        - std::pow(std::sin(params.i), 2) * std::sin(2 * u),
        - std::sin(2 * params.i) * std::sin(u)
    };
}

linear_algebra::Vector atmos(const helper::container::KeplerParameters& params,
                                    double rho_atm,
                                    double sail_area,
                                    const linear_algebra::Vector& sail_norm,
                                    const double mu, const double r, const double nu
) {
    double u = params.omega + nu;
    double ex =  params.e * std::cos(params.omega);
    double ey =  params.e * std::sin(params.omega);
    std::cout << "scaling: " << std::sqrt(mu / params.p) << std::endl;
    linear_algebra::Vector v = std::sqrt(mu / params.p) * linear_algebra::Vector {
            ex * std::sin(u) - ey * std::cos(u),
            1 + ex * std::cos(u) + ey * std::sin(u),
            0
    };

    linear_algebra::Vector v_earth = helper::constant::EARTH_OMEGA * r * linear_algebra::Vector {
            0,
            std::cos(params.i),
            std::sin(params.i) * std::cos(u)
    };

    linear_algebra::Vector v_eq  = v - v_earth;

    std::cout << "v: " << v << std::endl;
    std::cout << "v_earth: " << v_earth << std::endl;

    double scaling_factor = helper::constant::C_d * rho_atm * sail_area / 2 * std::fabs(v_eq * sail_norm);
    std::cout << "v_eq projection: " << std::fabs(v_eq * sail_norm) << std::endl;
    return - scaling_factor * v_eq;
}

linear_algebra::Vector solar(const helper::container::SailParameters& params, const linear_algebra::Vector& solar_norm) {
    double gamma = (params.ef * params.Bf - params.eb * params.Bb) / (params.ef + params.eb);
    double a1 = 1 - params.rho * params.s;
    double a2 = params.Bf * params.rho * (1 - params.s) + (1 - params.rho) * gamma;
    double a3 = params.rho * params.s;
    return helper::constant::SOLAR_PRESSURE  * (
            - a1 * (params.norm * solar_norm) * solar_norm
            + a2 * (params.norm * solar_norm) * params.norm
            - 2 * a3 * (params.norm * solar_norm) * (params.norm * solar_norm) * params.norm
          ) * params.area;
}

double atm_density(double h, double L) {
    return 4.8e-12;
}
}
