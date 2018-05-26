#pragma once

#include <objects/objects.h>
#include "LinearAlgebra.h"
#include <memory>
#include <shapes/shapes.h>
#include "mvp/actions/actions.h"

namespace math {
namespace model {
class Satellite: phys::object {
public:
    explicit Satellite(
            double m = 1.33,
            double width = 0.1,
            double height = 0.1,
            double length = 0.3
    );

    Satellite& sail(
            const helper::container::SailParameters& sailParams,
            const linear_algebra::Vector& r
    );

private:
    double m_width;
    double m_height;
    double m_length;

    helper::container::SailParameters& m_sailParams;
    linear_algebra::Vector& m_r;
};
}
}