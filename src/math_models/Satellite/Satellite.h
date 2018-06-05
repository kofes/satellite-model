#pragma once

#include <objects/objects.h>
#include "LinearAlgebra.h"
#include <memory>
#include <shapes/shapes.h>
#include "mvp/actions/actions.h"

namespace math {
namespace model {
class Satellite: public phys::object {
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

private:
    double m_width;
    double m_height;
    double m_length;

    helper::container::SailParameters m_sailParams;
    linear_algebra::Vector m_r;
};
}
}
