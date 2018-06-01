#include "phys_object.h"

namespace phys {

object::object(double mass) :
        m_dw(linear_algebra::Vector(3, 0)),
        m_dv(linear_algebra::Vector(3, 0)),
        m_mass(mass)
{}
object::~object() = default;

object& object::update_dw(linear_algebra::Vector& src) {
    m_dw = src;
    return *this;
}
object& object::update_dv(linear_algebra::Vector& src) {
    m_dv = src;
    return *this;
}

object& object::update(double dt) {
    this->move(m_dv * dt);
    this->rotate({1,0,0}, m_dw[0] * dt);
    this->rotate({0,1,0}, m_dw[1] * dt);
    this->rotate({0,0,1}, m_dw[2] * dt);
    return  *this;
}

linear_algebra::Vector object::dw() const {
    return m_dw;
}
linear_algebra::Vector object::dv() const {
    return m_dv;
}

double object::mass() const {
    return m_mass;
}
}