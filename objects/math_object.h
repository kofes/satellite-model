#pragma once

#include "LinearAlgebra.h"

namespace math {
class object {
public:
    object();
    virtual ~object();

    virtual object& rotate(const linear_algebra::Vector& axis, double angle);
    virtual object& orientation(const linear_algebra::Matrix& orientation);
    virtual object& scale(const linear_algebra::Vector& src);
    virtual object& move(const linear_algebra::Vector& dv);

    virtual linear_algebra::Vector position() const;
    virtual linear_algebra::Matrix scale() const;
    virtual linear_algebra::Matrix orientation() const;

protected:
    linear_algebra::Vector m_position;
    linear_algebra::Vector m_scale;
    linear_algebra::Matrix m_orientation;
};
}