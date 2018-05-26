#pragma once

#include <LinearAlgebra.h>
#include <helpers/integrals.h>
#include <helpers/helpers.h>

namespace kepler {
    class Differentials {
    public:
        /**
         * Format:
         * return d/dt [p, e, omega, Omega, i, tau]
         **/
        linear_algebra::Vector operator()(
                const helper::container::KeplerParameters& params,
                const phys::object& main,
                const phys::object& sat,
                const double t,
                const linear_algebra::Vector &force
        );

        inline double dexdt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double deydt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double dudt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double r(double p, double e, double omega, double u);
        inline double r(double p, double e, double nu);
    protected:
        Differentials() = default;
        Differentials(const Differentials&) = delete;
        ~Differentials() = default;
        typedef double (*Integral)(helper::integral::dfdt fun, double from, double to, size_t count);

        inline double dpdt(double r, double mu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double dedt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double domegadt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double didt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double dOmegadt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f);
        inline double dtaudt(double r, double mu, double nu, const helper::container::KeplerParameters& params, const linear_algebra::Vector &f, Integral integ = helper::integral::rect);
    };
}