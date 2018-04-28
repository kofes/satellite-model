#include "Satellite.h"

namespace math {
namespace model {

Satellite::Satellite(
        double m,
        double width,
        double height,
        double length
): phys::object(m)/*, shape_model(32, 32, 1)*/ {
//    shape_model.scale({length, height, width});
    m_width = width;
    m_height = height;
    m_length = length;
};
}
}