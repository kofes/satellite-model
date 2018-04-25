#pragma once

#include "KeplerDifferentials.h"

namespace force {
    struct SailParameters {
        double Bf, Bb;
        double ef, eb;
        double rho, s;
    };

    inline linear_algebra::Vector gravJ2(const Kepler::Parameters& params, double mass) {
        return 3 * helper::constant::J2 * params.mu * std::pow(helper::constant::EARTH_R, 2) / (2 * std::pow(params.r, 4)) * mass *
                linear_algebra::Vector {
            3 * std::pow(std::sin(params.i), 2) * std::pow(std::sin(params.u), 2) - 1,
            - std::pow(std::sin(params.i), 2) * std::sin(2 * params.u),
            - std::sin(2 * params.i) * std::sin(params.u)
        };
    }

    inline linear_algebra::Vector atmos(const Kepler::Parameters& params,
                                        double rho_atm,
                                        double sail_area,
                                        const linear_algebra::Vector& sail_norm
    ) {
        double ex =  params.e * std::cos(params.omega);
        double ey =  params.e * std::sin(params.omega);
        linear_algebra::Vector v_eq  = std::sqrt(params.mu / params.p) *
        linear_algebra::Vector {
               ex * std::sin(params.u) - ey * std::cos(params.u),
               1 + ex * std::cos(params.u) + ey * std::sin(params.u),
               0
        } - helper::constant::EARTH_OMEGA * params.r * linear_algebra::Vector {
            0,
            std::cos(params.i),
            std::sin(params.i) * std::cos(params.u)
        };
        return - helper::constant::C_d * rho_atm * sail_area / 2 * std::fabs(v_eq * sail_norm) * v_eq;
    }

    inline linear_algebra::Vector solar(
            const SailParameters& params,
            double sail_area,
            const linear_algebra::Vector& solar_norm,
            const linear_algebra::Vector& sail_norm) {
        double gamma = (params.ef * params.Bf - params.eb * params.Bb) / (params.ef + params.eb);
        return -helper::constant::SOLAR_PRESSURE * sail_area * ((
                2 * std::fabs(solar_norm * sail_norm) * params.rho * params.s
                + params.Bf * params.rho * (1 - params.s) + (1 - params.rho) * gamma) *
                (solar_norm * sail_norm) * sail_norm +
                (1 - params.rho * params.s) * std::fabs(solar_norm * sail_norm) * solar_norm
              );
    }

    static const double AtmPress0 = 101325; // Па
    static const double T0 = 288.15; // K
    static const double GravStat = 6.67408e-11; //м^3 кг^-1 с^-2

    double atm_density(double h, double L = -0.0065) {
//        double g = GravStat * helper::constant::EARTH_MASS / std::pow(h + helper::constant::EARTH_R, 2);
        double H = 8.5e+3;
//        double T = T0 + L * h/(helper::constant::EARTH_R);
//        double Molar_Mass = 0.0289644; // кг / моль
//        double UnivGasStat_R = 8.31447; // Дж / моль * K
        double p = AtmPress0 * std::exp(-h / H);

        return p;
    }
};
