#pragma once

#include "LinearAlgebra.h"
#include "mvp/actions/actions.h"
#include <list>

namespace math {
namespace model {
class Orbit {
public:
    Orbit(
            const linear_algebra::Vector& satellite_start_point,
            const linear_algebra::Vector& satellite_start_speed
    );

    Orbit& move_satellite(double dt = 1 /*sec*/);

    std::list<linear_algebra::Vector>& render(std::list<linear_algebra::Vector>& positions);
private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}
