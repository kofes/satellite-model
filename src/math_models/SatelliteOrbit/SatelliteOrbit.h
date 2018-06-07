#pragma once

#include <LinearAlgebra.h>
#include <mvp/actions/actions.h>
#include <list>
#include <objects/objects.h>
#include <memory>
#include <helpers/helpers.h>
#include <math_models/Satellite/Satellite.h>
#include <math_models/Earth/Earth.h>
#include <shapes/shapes.h>

namespace math {
namespace model {
class SatelliteOrbit {
public:
    explicit SatelliteOrbit(const double time = 0, const bool save_track = true);

    SatelliteOrbit(
        const math::model::Satellite& satellite,
        const helper::container::KeplerParameters& params,
        const double time = 0,
        const bool save_track = true);

    SatelliteOrbit& update(double dt = 1 /*sec*/);

    SatelliteOrbit& satellite(const math::model::Satellite& satellite);
    SatelliteOrbit& parameters(const helper::container::KeplerParameters& params);

    math::model::Satellite satellite();
    helper::container::KeplerParameters parameters();

    SatelliteOrbit& render(std::list<glsl::object>& result);

private:
    void resetSatellite();

    double m_time;

    math::model::Earth m_Earth;
    math::model::Satellite m_satellite;
    helper::container::KeplerParameters m_keplerParmeters;

    bool m_save_track;
    const size_t TRACK_COUNT_POINTS = 400;
    std::list<linear_algebra::Vector> m_track;
};
}
}
