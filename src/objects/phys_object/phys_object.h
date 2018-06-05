#pragma once

#include <objects/math_object/math_object.h>

namespace phys {
class object: public math::object {
public:
    explicit object(double mass);
    ~object() override;

    virtual object& update_dw(linear_algebra::Vector& src); //3 dimension
    virtual object& update_dv(linear_algebra::Vector& src); //3 dimension

    virtual object& update(double dt);

    virtual linear_algebra::Vector dw() const; //3 dimension
    virtual linear_algebra::Vector dv() const; //3 dimension
    virtual double mass() const; // kg
protected:
    linear_algebra::Vector m_dw;
    linear_algebra::Vector m_dv;
    double m_mass; // kg
};
}
