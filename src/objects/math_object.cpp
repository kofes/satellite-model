#include "mvp/actions/actions.h"
#include "math_object.h"

namespace math {
object::object() :
        m_position({0,0,0,1}),
        m_scale({1,1,1,1}),
        m_orientation(linear_algebra::Matrix::eye(4, 1))
{}

object::~object() = default;

object& object::rotate(const linear_algebra::Vector& axis, double angle) {
    m_orientation = mvp::action::rotate(axis, angle) * m_orientation;
    return *this;
}

object& object::orientation(const linear_algebra::Matrix& orientation) {
    m_orientation = orientation;
    return *this;
}

object& object::scale(const linear_algebra::Vector& src) {
    m_scale = mvp::action::scale(src) * m_scale;
    return *this;
}
object& object::move(const linear_algebra::Vector& dv) {
    m_position = mvp::action::translate(dv) * m_position;
    return *this;
}

linear_algebra::Vector object::position() const {
    return m_position;
}
linear_algebra::Matrix object::scale() const {
    return mvp::action::scale(m_scale);
}
linear_algebra::Matrix object::orientation() const {
    return m_orientation;
}
}