#include "projections.h"

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
) {
    if (src.width() != src.height() || src.width() != 4)
        return linear_algebra::Matrix();
    return linear_algebra::Matrix {
        {2 / (right - left),         0         ,         0        , - (right + left) / (right - left)},
        {        0         , 2 / (top - bottom),         0        , - (top + bottom) / (top - bottom)},
        {        0         ,         0         , -2 / (far - near),   - (far + near) / (far - near)  },
        {        0         ,         0         ,         0        ,                  1               }
    } * src;
}

linear_algebra::Matrix perspective(
        const linear_algebra::Matrix &src,
        double left,
        double right,
        double top,
        double bottom,
        double near,
        double far
) {
    if (src.width() != src.height() || src.width() != 4)
        return linear_algebra::Matrix();
    return linear_algebra::Matrix {
                   {2 * near / (right - left),            0             , (right + left) / (right - left),                0               },
                   {            0            , 2 * near / (top - bottom), (top + bottom) / (top - bottom),                0               },
                   {            0            ,            0             , - (far + near) / (far - near)  , - 2 * far * near / (far - near)},
                   {            0            ,            0             ,               -1               ,                0               }
           } * src;
}
}
}