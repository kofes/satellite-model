#include "SatelliteOrbit.h"

namespace math {
namespace model {
SatelliteOrbit::SatelliteOrbit(const double time, const bool save_track):
    m_time(time), m_save_track(save_track) {}

SatelliteOrbit::SatelliteOrbit(
    const math::model::Satellite& satellite,
    const helper::container::KeplerParameters& params,
    const double time, const bool save_track):
    m_time(time), m_save_track(save_track),
    m_satellite(satellite), m_keplerParmeters(params) {

    // initialize satellite's track
    if (m_save_track)
        m_track.clear();
}

SatelliteOrbit& SatelliteOrbit::satellite(const math::model::Satellite& satellite) {
    m_satellite = satellite;

    // initialize satellite's track
    if (m_save_track)
        m_track.clear();
}

SatelliteOrbit& SatelliteOrbit::parameters(const helper::container::KeplerParameters& params) {
    m_keplerParmeters = params;

    // initialize satellite's track
    if (m_save_track)
        m_track.clear();
}

SatelliteOrbit& SatelliteOrbit::update(double dt /*sec*/) {
    m_time += dt;

    double E = helper::orbit::E(m_keplerParmeters, m_Earth.mass(), m_satellite.mass(), m_time);
    double nu = helper::orbit::nu(m_keplerParmeters, E);
    double scalarR = helper::orbit::r(m_keplerParmeters, E);

    linear_algebra::Vector r = helper::orbit::r(scalarR, nu);
    r = helper::orbit::r(m_keplerParmeters, r);

    std::cout << "r: " << r << std::endl;

    // m_satellite.position(r);

    // update satellite's track
    if (m_save_track) {
        // remove last point & add new
        // m_track.pop_front();
    //     m_track.emplace_back(m_satellite.position());
    }
    return *this;
}

math::model::Satellite SatelliteOrbit::satellite() {
    return m_satellite;
}

helper::container::KeplerParameters SatelliteOrbit::parameters() {
    return m_keplerParmeters;
}

SatelliteOrbit& SatelliteOrbit::render(std::list<glsl::object>& result) {
    result.clear();
    double scalingFactor = helper::constant::EARTH_R / 2;
    auto center = shape::solid::sphere(32, 32, 1);
    center.move(linear_algebra::Vector {
                    m_Earth.position()[0], // x
                    m_Earth.position()[1], // y
                    m_Earth.position()[2]  // z
            })
            .scale(linear_algebra::Vector {
                    m_Earth.scale()[0][0], // x
                    m_Earth.scale()[1][1], // y
                    m_Earth.scale()[2][2]  // z
            } / scalingFactor / 2
            );
    center.orientation(m_Earth.orientation());

    center.update_color(helper::color(150, 50, 50));
    result.emplace_back((glsl::object)center);

    return *this;
}

void SatelliteOrbit::resetSatellite() {
    if (m_satellite.mass() <= 0)
        return;
    // initialize satellite's track
    if (m_save_track)
        m_track.clear();

    // calculate radius-vector for satellite
    double E = helper::orbit::E(m_keplerParmeters, m_Earth.mass(), m_satellite.mass(), m_time);
    double nu = helper::orbit::nu(m_keplerParmeters, E);
    double scalarR = helper::orbit::r(m_keplerParmeters, E);
    linear_algebra::Vector r = helper::orbit::r(scalarR, nu);
    r = helper::orbit::r(m_keplerParmeters, r);

    m_satellite.position(r);
}

}   // namespace model
}   // namespace math
