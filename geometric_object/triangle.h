#pragma once

#include <cstdlib>
#include <memory>

#include "geometric_object.h"
#include "LinearAlgebra.h"

namespace geometry {
class triangle : public object {
public:
    triangle(
        const linear_algebra::Vector& A,
        const linear_algebra::Vector& B,
        const linear_algebra::Vector& C
    );

    triangle& scale(const linear_algebra::Vector& scale);
    triangle& translate(const linear_algebra::Vector& move);
    triangle& rotate(const linear_algebra::Vector& axis, double degree);

    triangle& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}