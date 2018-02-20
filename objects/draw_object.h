#pragma once

#include <helpers/helpers.h>
#include "math_object.h"

namespace draw {
class object: math::object {
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
