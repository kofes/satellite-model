#include "orbit.h"

namespace helper {
    namespace orbit {
        double E(
                const helper::container::KeplerParameters& kepParams,
                const phys::object& main,
                const phys::object& sat,
                double t, double epsilon = 1e-5
        ) {
            double mu = helper::constant::G * main.mass();
            double a = kepParams.p / (1 - std::pow(kepParams.e, 2));
            double n = std::sqrt(mu / (a*a*a));

            double M = n * (t - kepParams.tau);
            double E1 = 0, E0 = 0;
            do {
                double tmp = E1;
                E1 = kepParams.e * std::sin(E0) + M;
                E0 = tmp;
            } while (std::fabs(E1 - E0) > epsilon);

            return E1;
        }

        double nu(const helper::container::KeplerParameters& kepParams, double E) {
            double cosNu = (std::cos(E) - kepParams.e) / (1 - kepParams.e * std::cos(E));
            double sinNu = std::sin(E) / (1 - kepParams.e * std::cos(E)) * std::sqrt(1 - std::pow(kepParams.e, 2));
            return std::atan2(sinNu, cosNu);
        }

        double r(const helper::container::KeplerParameters& kepParams, double E) {
            double a = kepParams.p / (1 - std::pow(kepParams.e, 2));
            return a * (1 - kepParams.e * std::sin(E));
        }

        linear_algebra::Vector r(double r, double nu) {
            return r * linear_algebra::Vector {std::cos(nu), std::sin(nu), 0};
        }

        linear_algebra::Vector r(
                const helper::container::KeplerParameters& kepParams,
                const linear_algebra::Vector& r
        ) {
            auto R = r;
            R.resize(4, 1);
            return mvp::action::rotate({0, 0, 1}, kepParams.omega) *
                   mvp::action::rotate({1, 0, 0}, kepParams.i) *
                   mvp::action::rotate({0, 0, 1}, kepParams.Omega) *
                   R;
        }

        double T(
                const helper::container::KeplerParameters& kepParams,
                const phys::object& main,
                const phys::object& sat
        ) {
            double mu = constant::G * main.mass();
            double a = kepParams.p / (1 - std::pow(kepParams.e, 2));
            double n = std::sqrt(mu / (a*a*a));

            return 2 * M_PI / n;
        }
    }
}