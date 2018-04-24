#include "Sun.h"

namespace math {
namespace model {
Sun::Sun(): phys::object(1.98855e+30) {
    this->scale({m_R, m_R, m_R});
}

double Sun::R() const {
    return m_R;
}
}
}