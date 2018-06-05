#pragma once

#include "../math_object/math_object.h"
#include "helpers/helpers.h"

namespace draw {
class object: public math::object {
public:
    object();
    ~object() override;

    virtual object& update_color(const helper::color& color);

    virtual object& model(linear_algebra::Matrix& res);
    virtual object& color(helper::color& res);

protected:
    helper::color m_color;
};
}
