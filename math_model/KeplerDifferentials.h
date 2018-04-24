#pragma once

#include <inc/Vector.h>
#include <helpers/integrals.h>

namespace Kepler {
    struct Parameters {
        double p = 1;
        double mu = 1;
        double e = 1;
        double nu = 0;
        double omega = 0;
        double Omega = 0;
        double u = 0;
        double i = 0;
        double r = 1;
    };

    inline double dpdt(const Parameters& params, const linear_algebra::Vector &f) {
        return 2 * params.r * std::sqrt(params.p / params.mu) * f[1];
    }

    inline double dedt(const Parameters& params, const linear_algebra::Vector &f) {
        return std::sqrt(params.p / params.mu) *
               (f[0] * std::sin(params.nu) + f[1] * ((1 + params.r / params.p)
                                                     * std::cos(params.nu) + params.e * params.r / params.p));
    }

    inline double domegadt(const Parameters& params, const linear_algebra::Vector &f) {
        return 1 / params.e * std::sqrt(params.p / params.mu) * (
                - f[0] * std::cos(params.nu)
                + f[1] * (1 + params.r / params.p) * std::sin(params.nu)
                - f[2] * params.e * params.r / params.p * std::sin(params.u) / std::tan(params.i)
                );
    }

    inline double didt(const Parameters& params, const linear_algebra::Vector &f) {
        return params.r / std::sqrt(params.mu * params.p) * std::cos(params.u) * f[2];
    }

    inline double dOmegadt(const Parameters& params, const linear_algebra::Vector &f) {
        return params.r / std::sqrt(params.mu * params.p) * std::sin(params.u) / std::sin(params.i) * f[2];
    }

    typedef double (*Integral)(integral::dfdt fun, double from, double to, size_t count);

    inline double dtaudt(const Parameters& params, const linear_algebra::Vector &f, Integral integ = integral::rect) {
        auto& Int = integ;
        auto fun = [&](double dnu) -> double {
            return 2 * std::cos(dnu) / std::pow(1 + params.e / std::cos(dnu), 3);
        };
        double N = params.p * params.p / params.r / params.r * Int(fun, 0, params.nu, 1000);
        return params.r * params.r / (params.e * params.mu) * (
                (params.e * N * std::sin(params.nu) - std::cos(params.nu)) * f[0] +
                N * params.p / params.r * f[1]
                );
    }

    inline double dexdt(const Parameters& params, const linear_algebra::Vector &f) {
        double ex =  params.e * std::cos(params.omega);
        double ey =  params.e * std::sin(params.omega);

        return std::sqrt(params.p / params.mu) * (
                 f[0] * std::sin(params.u)
                +f[1] * ((1 + params.r / params.p)*std::cos(params.u) + params.r * ex / params.p)
                +f[2] * params.r * ey / params.p * std::cos(params.i) * std::sin(params.u)
                );
    }

    inline double deydt(const Parameters& params, const linear_algebra::Vector &f) {
        double ex =  params.e * std::cos(params.omega);
        double ey =  params.e * std::sin(params.omega);

        return std::sqrt(params.p / params.mu) * (
                -f[0] * std::cos(params.u)
                +f[1] * ((1 + params.r / params.p)*std::sin(params.u) + params.r * ey / params.p)
                -f[2] * params.r * ex / params.p * std::cos(params.i) * std::sin(params.u)
        );
    }

    inline double dudt(const Parameters& params, const linear_algebra::Vector &f) {
        return std::sqrt(params.mu * params.p) / (params.r * params.r)
               - f[2] * params.r * std::cos(params.i) * std::sin(params.u)
                 / std::sqrt(params.mu * params.p);
    }

    inline double r(double p, double e, double omega, double u) {
        double ex =  e * std::cos(omega);
        double ey =  e * std::sin(omega);
        return p / (1 + ex * std::cos(u) + ey * std::sin(u));
    }

    inline double r(double p, double e, double nu) {
        return p / (1 + e * std::cos(nu));
    }
}