#pragma once

#include "math_object.h"

namespace phys {
class object: public math::object {
public:
    explicit object(double mass);
    ~object() override;

    virtual object& update_dw(linear_algebra::Vector& src); //3 dimension
    virtual object& update_dv(linear_algebra::Vector& src); //3 dimension

    virtual object& update(double dt);

    virtual object& dw(linear_algebra::Vector& res); //3 dimension
    virtual object& dv(linear_algebra::Vector& res); //3 dimension
    virtual object& mass(double& mass); // kg
protected:
    linear_algebra::Vector m_dw;
    linear_algebra::Vector m_dv;
    double m_mass; // kg
};
}