#include "actions.h"
#include <cmath>

namespace mvp {
namespace action {
linear_algebra::Matrix translate(const linear_algebra::Vector& move) {
    if (move.size() < 3)
        return linear_algebra::Matrix();
    return {
            {1, 0, 0, move[0]},
            {0, 1, 0, move[1]},
            {0, 0, 1, move[2]},
            {0, 0, 0,    1   }
    };
}

linear_algebra::Matrix rotate(const linear_algebra::Vector& axis, double degree) {
    if (axis.size() < 3)
        return linear_algebra::Matrix();
    degree -= ((int)degree / 360) * 360;
    double c = std::cos(degree * M_PI / 180);
    double s = std::sin(degree * M_PI / 180);
    linear_algebra::Vector n = axis.normalize();
    return {
            {n[0] * n[0] * (1 - c) + c       , n[1] * n[0] * (1 - c) - n[2] * s, n[2] * n[0] * (1 - c) + n[1] * s, 0},
            {n[0] * n[1] * (1 - c) + n[2] * s, n[1] * n[1] * (1 - c) + c       , n[2] * n[1] * (1 - c) - n[0] * s, 0},
            {n[0] * n[2] * (1 - c) - n[1] * s, n[1] * n[2] * (1 - c) + n[0] * s, n[2] * n[2] * (1 - c) + c       , 0},
            {               0                ,                0                ,                0                , 1}
    };
}

linear_algebra::Matrix scale(const linear_algebra::Vector& scale) {
    if (scale.size() < 3)
        return linear_algebra::Matrix();
    return {
            {scale[0],    0    ,    0    , 0},
            {   0    , scale[1],    0    , 0},
            {   0    ,    0    , scale[2], 0},
            {   0    ,    0    ,    0    , 1}
    };
}
}
}