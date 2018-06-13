#include "projections.h"

namespace mvp {
namespace projection {
linear_algebra::Matrix ortho(
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
) {
    return linear_algebra::Matrix {
        {2 / (right - left),                  0,                 0, - (right + left) / (right - left)},
        {                 0, 2 / (top - bottom),                 0, - (top + bottom) / (top - bottom)},
        {                 0,                  0, -2 / (far - near),     - (far + near) / (far - near)},
        {                 0,                  0,                 0,                                 1}
    };
}

linear_algebra::Matrix ortho(
        double width,
        double height,
        double near,
        double far
) {
    return linear_algebra::Matrix {
            {2 / width,          0,                 0,                             0},
            {        0, 2 / height,                 0,                             0},
            {        0,          0, -2 / (far - near), - (far + near) / (far - near)},
            {        0,          0,                 0,                             1}
    };
}

linear_algebra::Matrix perspective(
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
) {
    return linear_algebra::Matrix {
                   {2 * near / (right - left),            0             , (right + left) / (right - left),                0               },
                   {            0            , 2 * near / (top - bottom), (top + bottom) / (top - bottom),                0               },
                   {            0            ,            0             , - (far + near) / (far - near)  , - 2 * far * near / (far - near)},
                   {            0            ,            0             ,               -1               ,                0               }
           };
}

linear_algebra::Matrix perspective(
        double width,
        double height,
        double near,
        double far
) {
    return linear_algebra::Matrix {
            {2 * near / width,                 0,                             0,                               0},
            {               0, 2 * near / height,                             0,                               0},
            {               0,                 0, - (far + near) / (far - near), - 2 * far * near / (far - near)},
            {               0,                 0,                            -1,                               0}
    };
}
}
}