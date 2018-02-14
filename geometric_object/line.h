#pragma once
#pragma once

#include <cstdlib>
#include <memory>

#include "geometric_object.h"
#include "LinearAlgebra.h"

namespace geometry {
class line : public object {
public:
    line(
            const linear_algebra::Vector& A,
            const linear_algebra::Vector& B
    );

    line& color(uint8_t r, uint8_t g, uint8_t b);

    line& scale(const linear_algebra::Vector& scale);
    line& translate(const linear_algebra::Vector& move);
    line& rotate(const linear_algebra::Vector& axis, double degree);

    line& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}