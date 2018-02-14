#include "Earth.h"

namespace math {
namespace model {
class Earth::core {
public:
    core() = default;

    double R = 6371e+3; //m
    double M = 5.97237e+24; //kg
    linear_algebra::Vector position = {0,0,0,1}; //m
};

Earth::Earth(): m_core(new Earth::core) {};

double Earth::mass() const {
    return m_core->M;
}

double Earth::R() const {
    return m_core->R;
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