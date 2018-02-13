#pragma once

#include <cstdlib>
#include <memory>

#include "geometric_object.h"

namespace geometry {
class grid : public object {
public:
    grid(std::size_t count = 0);

    grid& count(std::size_t count);

    grid& render(const linear_algebra::Matrix& vp) override;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}