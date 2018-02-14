#pragma once

#include <memory>
#include "LinearAlgebra.h"
#include "mvp/actions/actions.h"

namespace math {
namespace model {
class Earth {
public:
    Earth();

    double mass() const;
    double R() const;
    const linear_algebra::Vector& position() const;

    linear_algebra::Matrix model() const;

private:
    class core;

    std::shared_ptr<core> m_core;
};
}
}