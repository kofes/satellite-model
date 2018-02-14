#pragma once

#include "LinearAlgebra.h"
#include "mvp/actions/actions.h"
#include <list>
#include <geometric_object/geometric_object.h>

namespace math {
namespace model {
class Orbit {
public:
    Orbit(
            const linear_algebra::Vector& satellite_start_point,
            const linear_algebra::Vector& satellite_start_speed
    );

    Orbit& move_satellite(double dt = 1 /*sec*/);

    std::list<std::shared_ptr<geometry::object>>& render(std::list<std::shared_ptr<geometry::object>>& objects);

private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}
