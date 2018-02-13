#pragma once

#include <cstdlib>
#include <memory>

#include "geometric_object.h"
#include "LinearAlgebra.h"

namespace geometry {
    class quadrilateral : public object {
    public:
        quadrilateral(
                const linear_algebra::Vector& A,
                const linear_algebra::Vector& B,
                const linear_algebra::Vector& C,
                const linear_algebra::Vector& D
        );

        quadrilateral& render(const linear_algebra::Matrix& vp) override;

    private:
        class core;

        std::shared_ptr<core> m_core;
    };
}