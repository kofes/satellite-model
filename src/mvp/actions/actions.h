#pragma once

#include "LinearAlgebra.h"

namespace mvp {
namespace action {
linear_algebra::Matrix translate(const linear_algebra::Vector& move);

linear_algebra::Matrix rotate(const linear_algebra::Vector& axis, double degree);

linear_algebra::Matrix R_x(double degree);
linear_algebra::Matrix R_y(double degree);
linear_algebra::Matrix R_z(double degree);

linear_algebra::Matrix scale(const linear_algebra::Vector& scale);
}
}
