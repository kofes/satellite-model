#include "Satellite.h"

namespace math {
namespace model {

Satellite::Satellite(
        double m,
        double width,
        double height,
        double length
): m_sailParams(), m_sail_r(),
   m_angles(linear_algebra::Vector(3)),
      m_position(linear_algebra::Vector(4, 0)) {
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
    m_angles = src;
    m_angles.resize(3);
    return *this;
}
Satellite& Satellite::position(const linear_algebra::Vector& src) {
    if (src.size() < 3)
        return *this;
    m_position = src;
    m_position.resize(4, 1);
    return *this;
}

linear_algebra::Vector& Satellite::angles() {
    return m_angles;
}

linear_algebra::Vector& Satellite::position() {
    return m_position;
}

linear_algebra::Matrix Satellite::orientation() const {
    return linear_algebra::Matrix {
        {std::cos(m_angles[0]) * std::cos(m_angles[1]), std::sin(m_angles[1]), -std::sin(m_angles[0]) * std::cos(m_angles[1]), 0},
            {
                -std::cos(m_angles[0]) * std::sin(m_angles[1]) * std::cos(m_angles[2]) + std::sin(m_angles[0]) * std::sin(m_angles[2]),
                 std::cos(m_angles[1]) * std::cos(m_angles[2]),
                 std::sin(m_angles[0]) * std::sin(m_angles[1]) * std::cos(m_angles[2]) + std::cos(m_angles[0]) * std::sin(m_angles[2]),
                 0
             },
             {
                 std::cos(m_angles[0]) * std::sin(m_angles[1]) * std::sin(m_angles[2]) + std::sin(m_angles[0]) * std::cos(m_angles[2]),
                 -std::cos(m_angles[1]) * std::sin(m_angles[2]),
                 -std::sin(m_angles[0]) * std::sin(m_angles[1]) * std::sin(m_angles[2]) + std::cos(m_angles[0]) * std::cos(m_angles[2]),
                 0
             },
        {0, 0, 0, 1}
    };
}

}
}
