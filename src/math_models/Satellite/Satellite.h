#pragma once

#include "LinearAlgebra.h"
#include <memory>
#include "mvp/actions/actions.h"
#include <helpers/helpers.h>

namespace math {
namespace model {
class Satellite {
public:
    explicit Satellite(
            double m = 1.33,
            double width = 0.1,
            double height = 0.1,
            double length = 0.3
    );

    Satellite(const Satellite& src);

    Satellite& sail(
            const helper::container::SailParameters& sailParams,
            const linear_algebra::Vector& r
    );

    helper::container::SailParameters sailParameters();
    linear_algebra::Vector sailR();

    double mass() const;    // kg

    Satellite& angles(const linear_algebra::Vector& src);   // 3-dimension
    Satellite& position(const linear_algebra::Vector& src); // 3-dimension

    linear_algebra::Vector& angles();   // 3-dimension
    linear_algebra::Vector& position(); // 3-dimension

private:
    double m_mass;

    double m_width;
    double m_height;
    double m_length;

    linear_algebra::Vector m_angles;
    linear_algebra::Vector m_position;

    helper::container::SailParameters m_sailParams;
    linear_algebra::Vector m_sail_r;
};
}
}
