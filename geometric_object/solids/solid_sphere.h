#pragma once

#include <cstdlib>
#include <memory>

#include "geometric_object/geometric_object.h"
#include "LinearAlgebra.h"

namespace geometry {
namespace solid {
class sphere: public object {
public:
    sphere(float radius, size_t slices, size_t stacks);

    const linear_algebra::Vector& position() const;

    sphere& scale(const linear_algebra::Vector& scale);
    sphere& translate(const linear_algebra::Vector& move);
    sphere& rotate(const linear_algebra::Vector& axis, double degree);

    sphere& color(uint8_t r, uint8_t g, uint8_t b);

    sphere& show_normals(const linear_algebra::Matrix& vp) override;

    sphere& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}
