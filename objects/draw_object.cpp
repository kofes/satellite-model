#include "draw_object.h"
#include "mvp/actions/actions.h"

namespace draw {
object::object(): m_color(0,0,0) {};
object::~object() = default;

object& object::update_color(const helper::color& color) {
    m_color = color;
    return *this;
}

object& object::model(linear_algebra::Matrix& res) {
    res = m_orientation * mvp::action::scale(m_scale) * mvp::action::translate(m_position).T();
    return *this;
};
object& object::color(helper::color& res) {
    res = m_color;
    return *this;
};
}