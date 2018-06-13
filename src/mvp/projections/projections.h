#pragma once

#include "LinearAlgebra.h"

namespace mvp {
namespace projection {
linear_algebra::Matrix ortho(
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
);

linear_algebra::Matrix ortho(
        double width,
        double height,
        double near,
        double far
);

linear_algebra::Matrix perspective(
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
);

linear_algebra::Matrix perspective(
        double width,
        double height,
        double near,
        double far
);
}
}
