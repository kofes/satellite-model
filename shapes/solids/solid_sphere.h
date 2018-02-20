#pragma once

#include <cstdlib>
#include <memory>
#include <objects/objects.h>
#include "LinearAlgebra.h"

namespace shape {
namespace solid {
class sphere: public glsl::object {
public:
    sphere(size_t slices, size_t stacks, float radius = 1);

    sphere& update_color(const helper::color& color) override;

    sphere& show_normals(const linear_algebra::Matrix& vp) override;

    sphere& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}
