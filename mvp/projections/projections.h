#pragma once

#include "LinearAlgebra.h"

namespace mvp {
namespace projection {
linear_algebra::Matrix ortho(
        const linear_algebra::Matrix &src,
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
);

linear_algebra::Matrix perspective(
        const linear_algebra::Matrix &src,
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
);
}
}
