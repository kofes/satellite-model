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

    sphere& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}
