#pragma once

#include "LinearAlgebra.h"

namespace math {
class object {
public:
    object();
    virtual ~object();

    virtual object& rotate(const linear_algebra::Vector& axis, double angle);
    virtual object& scale(const linear_algebra::Vector& src);
    virtual object& move(const linear_algebra::Vector& dv);

    virtual object& position(linear_algebra::Vector& res);
    virtual object& scale(linear_algebra::Matrix& res);
    virtual object& orientation(linear_algebra::Matrix& res);

protected:
    linear_algebra::Vector m_position;
    linear_algebra::Vector m_scale;
    linear_algebra::Matrix m_orientation;
};
}