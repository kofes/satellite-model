#pragma once

#include "LinearAlgebra.h"

#include "mvp/actions/actions.h"

namespace math {
namespace model {
class Satellite {
public:
    Satellite(
            double m = 1.33,
            double width = 0.1,
            double height = 0.1,
            double length = 0.3
    );

    linear_algebra::Matrix model() const;

    Satellite& move(const linear_algebra::Vector& mv);

    const linear_algebra::Vector& position() const;
    double mass() const;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}