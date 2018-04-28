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
    double m_R = 6371e+3; //m
    double m_pR = 6356.8e+3; //m
    double m_eR = 6378.1e+3; //m
};
}
}