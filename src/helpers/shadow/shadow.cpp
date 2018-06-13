#include "shadow.h"
#include "../helpers.h"

namespace helper {
namespace shadow {
// angle btw satellite-radial vector & Sun-Earth-radial
double angle(
        const linear_algebra::Vector& r,
        const linear_algebra::Vector& rSE
) {
    auto tR = r;
    auto tRSE = rSE;
    tR.resize(3);
    tRSE.resize(3);

    tR.normalizeInPlace();
    tRSE.normalizeInPlace();

    return tR * tRSE;
}

// angle of <<half-shadow>>
double shadow2Angle(
        const linear_algebra::Vector& r,
        const linear_algebra::Vector& rSE
) {
    auto tR = r;
    auto tRSE = rSE;
    tR.resize(3);
    tRSE.resize(3);

    double tgOmega = (helper::constant::SUN_RADIUS + helper::constant::EARTH_R) / rSE.length();

    return shadowAngleThrowOmega(tR, tRSE, tgOmega);
}

// angle of <<shadow>>
double shadowAngle(
        const linear_algebra::Vector& r,
        const linear_algebra::Vector& rSE
) {
    auto tR = r;
    auto tRSE = rSE;
    tR.resize(3);
    tRSE.resize(3);

    double tgOmega = (helper::constant::SUN_RADIUS - helper::constant::EARTH_R) / rSE.length();

    return shadowAngleThrowOmega(tR, tRSE, tgOmega);
}

// common part of evaluation of shadow angle
double shadowAngleThrowOmega(
        const linear_algebra::Vector& r,
        const linear_algebra::Vector& rSE,
        double tgOmega) {
    double x = (rSE.length() + helper::constant::SUN_RADIUS * tgOmega + std::sqrt(
            (tgOmega*tgOmega + 1) * r.length() * r.length()
            -std::pow(helper::constant::SUN_RADIUS - rSE.length() * tgOmega, 2)
            )) / (tgOmega * tgOmega + 1);
    double y = - helper::constant::SUN_RADIUS + tgOmega * x;

    return std::atan(y / (x - rSE.length()));
}

// function of shadow
double shadowFunction(
            const linear_algebra::Vector& r,
            const linear_algebra::Vector& rSE
    ) {
    double ang = angle(r, rSE);
    double sAng = shadowAngle(r, rSE);
    double s2Ang = shadow2Angle(r, rSE);

    return 1 / (1 + std::exp(-2 * (ang - sAng) / (s2Ang - sAng)));
}
}   // namespace shadow
}   // namespace helper
