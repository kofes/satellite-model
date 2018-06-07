#include "Satellite.h"

namespace math {
namespace model {

Satellite::Satellite(
        double m,
        double width,
        double height,
        double length
): m_sailParams(), m_sail_r() {
    m_mass = m;
    m_width = width;
    m_height = height;
    m_length = length;
};

Satellite::Satellite(const Satellite& src) {
    m_mass = src.m_mass;
    m_width = src.m_width;
    m_height = src.m_height;
    m_length = src.m_length;

    m_angles = src.m_angles;
    m_position = src.m_position;

    m_sailParams = src.m_sailParams;
    m_sail_r = src.m_sail_r;
}

Satellite& Satellite::sail(
        const helper::container::SailParameters& sailParams,
        const linear_algebra::Vector& r
) {
    m_sailParams = sailParams;
    m_sail_r = r;
    return *this;
}

helper::container::SailParameters Satellite::sailParameters() {
    return m_sailParams;
}

linear_algebra::Vector Satellite::sailR() {
    return m_sail_r;
}

double Satellite::mass() const {
    return m_mass;
}

Satellite& Satellite::angles(const linear_algebra::Vector& src) {
    if (src.size() < 3)
        return *this;
    for (size_t i = 0; i < src.size(); ++i)
        m_angles[i] = src[i];
    return *this;
}
Satellite& Satellite::position(const linear_algebra::Vector& src) {
    if (src.size() < 3)
        return *this;
    for (size_t i = 0; i < src.size(); ++i)
        m_position[i] = src[i];
    return *this;
}

linear_algebra::Vector& Satellite::angles() {
    return m_angles;
}

linear_algebra::Vector& Satellite::position() {
    return m_position;
}


}
}
