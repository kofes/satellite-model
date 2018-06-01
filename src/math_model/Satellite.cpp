#include "Satellite.h"

namespace math {
namespace model {

Satellite::Satellite(
        double m,
        double width,
        double height,
        double length
): phys::object(m),
   m_sailParams(),
   m_r()
{
    scale({width, length, height});
    m_width = width;
    m_height = height;
    m_length = length;
};

Satellite& Satellite::sail(
        const helper::container::SailParameters& sailParams,
        const linear_algebra::Vector& r
) {
    m_sailParams = sailParams;
    m_r = r;
}

}
}