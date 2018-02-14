#include "Earth.h"

namespace math {
namespace model {
class Earth::core {
public:
    core() = default;

    double R = 6371e+3; //m
    double pR = 6356.8e+3; //m
    double eR = 6378.1e+3; //m
    double M = 5.97237e+24; //kg
    linear_algebra::Vector position = {
            0, // -x
            0,  // -y
            0,  // z
            1}; //m
};

Earth::Earth(): m_core(new Earth::core) {};

double Earth::mass() const {
    return m_core->M;
}

double Earth::R() const {
    return m_core->R;
}

double Earth::eR() const {
    return m_core->eR;
}

double Earth::pR() const {
    return m_core->pR;
}

const linear_algebra::Vector& Earth::position() const {
    return m_core->position;
}

linear_algebra::Matrix Earth::model() const {
    double& R = m_core->R;
    return mvp::action::scale({R, R, R}) *
           mvp::action::translate(m_core->position);
}
}
}