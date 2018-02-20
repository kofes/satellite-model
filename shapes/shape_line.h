#pragma once

#include <cstdlib>
#include <memory>
#include <objects/objects.h>

#include "LinearAlgebra.h"

namespace shape {
class line : public glsl::object {
public:
    line(
            const linear_algebra::Vector& A,
            const linear_algebra::Vector& B
    );

    line& update_color(const helper::color& color) override;

    line& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}