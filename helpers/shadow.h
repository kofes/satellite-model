#pragma once

#include <LinearAlgebra.h>

namespace helper {
namespace shadow {
    // angle btw satellite-radial vector & Sun-Earth-radial
    double angle(
        const linear_algebra::Vector& r,
        const linear_algebra::Vector& rSE
    );

    // angle of <<half-shadow>>
    double shadow2Angle(
            const linear_algebra::Vector& r,
            const linear_algebra::Vector& rSE
    );

    // angle of <<shadow>>
    double shadowAngle(
            const linear_algebra::Vector& r,
            const linear_algebra::Vector& rSE
    );

    // common part of evaluation of shadow angle
    double shadowAngleThrowOmega(
            const linear_algebra::Vector& r,
            const linear_algebra::Vector& rSE,
            double tgOmega);

    // function of shadow
    double shadowFunction(
            const linear_algebra::Vector& r,
            const linear_algebra::Vector& rSE
    );
}
}