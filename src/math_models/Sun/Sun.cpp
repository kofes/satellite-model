#include "Sun.h"

namespace math {
namespace model {
Sun::Sun(): phys::object(helper::constant::SUN_MASS) {
    this->scale({m_R, m_R, m_R});
}

double Sun::R() const {
    return m_R;
}
}
}