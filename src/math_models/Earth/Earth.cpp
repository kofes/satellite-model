#include "Earth.h"

namespace math {
namespace model {

Earth::Earth(): phys::object(helper::constant::EARTH_MASS) {
    this->scale({m_eR, m_eR, m_pR});
}

double Earth::R() const {
    return m_R;
}

double Earth::eR() const {
    return m_eR;
}

double Earth::pR() const {
    return m_pR;
}
}
}
