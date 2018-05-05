#include "orbit.h"

namespace helper {
    namespace orbit {
        double anomaly(double dt, double p, double e, double mu) {
            double a = p / (1 - e * e);
            double M = std::sqrt(mu / std::pow(a, 3)) * dt;
            double E = M, E_0;

            do {
                E_0 = E;
                E = e * std::sin(E_0) + M;
            } while (std::fabs(E - E_0) > 1e-5);

            return E;
        }

        std::tuple<double, double> ellipticPolarCoordinates(double dt, double p, double e, double mu) {
            double E = anomaly(dt, p, e, mu);
            double a = p / (1 - e * e);
            double tmp_E = 1 - e * std::cos(E);

            double r = a * tmp_E;
            double c_nu = (std::cos(E) - e) / tmp_E;
            double s_nu = std::sin(E) * std::sqrt(1 - e * e) / tmp_E;
            return std::make_tuple(r, std::atan2(s_nu, c_nu));
        };

        double shadow(
                const helper::container::KeplerParameters& keplerParameters,
                const linear_algebra::Vector& solar_norm,
                const helper::container::SailParameters& sailParameters
        ) {
            if (std::fabs(solar_norm[0] - std::sqrt(1 - std::pow(helper::constant::EARTH_R / keplerParameters.r, 2))))
                return 0;
            return 1;
        }
    }
}