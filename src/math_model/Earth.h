#pragma once

#include <memory>
#include <objects/objects.h>
#include <shapes/shapes.h>
#include "LinearAlgebra.h"
#include "mvp/actions/actions.h"

namespace math {
namespace model {
class Earth: phys::object {
public:
    Earth();

    double R() const;
    double eR() const;
    double pR() const;

private:
    double m_R = helper::constant::EARTH_R; //m
    double m_pR = helper::constant::EARTH_P_R; //m
    double m_eR = helper::constant::EARTH_E_R; //m
};
}
}