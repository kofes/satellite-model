#pragma once

#include <objects/objects.h>

namespace math {
namespace model {
class Sun: phys::object {
public:
    Sun();

    double R() const;

private:
    double m_R = helper::constant::SUN_RADIUS;

};
}
}
