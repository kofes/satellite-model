#include "Satellite.h"

namespace math {
namespace model {
class Satellite::core {
public:
    core() = default;

    double m; //kg
    double width, height, length;
    linear_algebra::Vector position = {0,0,0,1};
};

Satellite::Satellite(
        double m,
        double width,
        double height,
        double length
): m_core(new Satellite::core) {
    m_core->m = m;
    m_core->width = width;
    m_core->height = height;
    m_core->length = length;
};

double Satellite::mass() const {
    return m_core->m;
}

linear_algebra::Matrix Satellite::model() const {
    double& width = m_core->width;
    double& height = m_core->height;
    double& length = m_core->length;
    return mvp::action::scale({width, height, length}) *
           mvp::action::translate(m_core->position);
}

const linear_algebra::Vector& Satellite::position() const {
    return m_core->position;
}

Satellite& Satellite::move(const linear_algebra::Vector& mv) {
    m_core->position = mvp::action::translate(mv) * m_core->position;
    return *this;
}


}
}