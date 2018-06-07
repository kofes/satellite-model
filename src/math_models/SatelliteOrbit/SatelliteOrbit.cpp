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

    updateParameters(scalarR, nu, dt);

    linear_algebra::Vector r = helper::orbit::r(scalarR, nu);

    r = helper::orbit::r(m_keplerParmeters, r);

    std::cout << "r: " << r << std::endl;

    m_satellite.position(r);

    // update satellite's track
    if (m_save_track) {
        // remove last point & add new
        m_track.pop_front();
        m_track.emplace_back(m_satellite.position());
    }
    return *this;
}

void SatelliteOrbit::updateParameters(double& sclrR, double& nu, double dt, double dh) {
    helper::container::KeplerParameters v_kParameters;
    v_kParameters.p = m_keplerParmeters.p;
    v_kParameters.e = m_keplerParmeters.e;
    v_kParameters.omega = m_keplerParmeters.omega * M_PI / 180;
    v_kParameters.Omega = m_keplerParmeters.Omega * M_PI / 180;
    v_kParameters.i = m_keplerParmeters.i * M_PI / 180;
    v_kParameters.tau = m_keplerParmeters.tau;

    double v_mu = helper::constant::G * (m_Earth.mass() + m_satellite.mass());
    double v_nu = nu;
    double v_u = nu + m_keplerParmeters.omega * M_PI / 180;
    double v_r = sclrR;

    size_t N = std::ceil(dt / dh);

    // initialize solar parameters
    double v_lambda = 90;
    double v_angleScalingFactor = 4;

    for (size_t i = 0; i <= N; ++i) {
        // TODO:
        // make solar normal vector !at global orientation system!
        linear_algebra::Vector sigma =
                mvp::action::R_x(-v_kParameters.i) *
                mvp::action::R_z(-v_kParameters.Omega) *
                mvp::action::R_x(helper::constant::EARTH_ECLIPTIC) *
                mvp::action::R_z(v_lambda)
                * linear_algebra::Vector {1, 0, 0, 1};

        double theta_s = std::acos(sigma[0]);
        double psi_s = std::atan2(
                sigma[0] * std::cos(v_u) + sigma[1] * std::sin(v_u),
                sigma[0] * std::sin(v_u) - sigma[1] * std::cos(v_u)
        );

        linear_algebra::Vector solar_norm {
                std::sin(psi_s) * std::sin(theta_s),
                std::cos(psi_s) * std::sin(theta_s),
                std::cos(theta_s)
        };

        // intialize main orientation system
        linear_algebra::Matrix orbital_orientation =
                    mvp::action::R_z(v_kParameters.Omega) *
                    mvp::action::R_x(v_kParameters.i) *
                    mvp::action::R_z(v_kParameters.omega) *
                    mvp::action::R_z(v_nu);

        // reorient sail norm to orbital orientation system
        orbital_orientation.resize(3, 0);
        auto sail_norm = orbital_orientation * m_satellite.sailParameters().norm; // to orbital orientation system
        //
        // TODO: find out optimization angles
        linear_algebra::Vector res_angles = helper::optimization::amoeba([&] (const linear_algebra::Vector& vec) {
            auto v_angles = m_satellite.angles();
            v_angles[0] += x[0];
            v_angles[1] += x[1];
            v_angles[2] += x[2];
            // intialize linked orientation system
            linear_algebra::Matrix satellite_orientation =
                        mvp::action::R_x(v_angles[2]) * // gamma
                        mvp::action::R_z(v_angles[1]) * // beta
                        mvp::action::R_y(v_angles[0]);  // alpha
            // reorient sail norm to global orientation system
            satellite_orientation.resize(3, 0);
            auto params = m_satellite.sailParameters();
            params.norm = satellite_orientation * sail_norm; // to global orientation system

            auto force =
                         force::solar(params, solar_norm)
                       // + force::atmos(v_physObjectKeplerParameters, 4.8e-12, params.area, params.norm, mu, r, nu)
                       ;

            double result = (std::sin(nu)) * force[0]
            + (2*r + (1 + r/v_kParameters.p) * std::cos(nu) + v_kParameters.e * r / v_kParameters.p) * force[1]
            result /= m_satellite.mass();

            return result;
       },
           true,
           3,
           linear_algebra::Vector(3, -M_PI),
           linear_algebra::Vector(3, M_PI),
           150
       );
       auto v_angles = m_satellite.angles();
       m_satellite.angles(v_angles + res_angles.normalize() * v_angleScalingFactor / M_PI);
       // TODO:
       linear_algebra::Matrix satellite_orientation =
                   mvp::action::R_x(m_satellite.angles()[2]) * // gamma
                   mvp::action::R_z(m_satellite.angles()[1]) * // beta
                   mvp::action::R_y(m_satellite.angles()[0]);  // alpha
       // reorient sail norm to global orientation system
       satellite_orientation.resize(3, 0);
       auto params = m_satellite.sailParameters();
       params.norm = satellite_orientation * sail_norm; // to global orientation system

       auto force =
                    force::solar(params, solar_norm)
                  // + force::atmos(v_physObjectKeplerParameters, 4.8e-12, params.area, params.norm, mu, r, nu)
                  ;
        //

       acceleration = force / m_satellite.mass();
       // update Kepler's orbital elements
       auto v_newkParams = helper::Differentials(v_kParameters, m_Earth.mass(), m_satellite.mass(), (m_time - dt) + i*dh, acceleration);
       v_kParameters.p += v_newkParams[0] * dh;
       v_kParameters.e += v_newkParams[1] * dh;
       v_kParameters.omega += v_newkParams[2] * dh;
       v_kParameters.i += v_newkParams[3] * dh;
       v_kParameters.Omega += v_newkParams[4] * dh;
       v_kParameters.tau += v_newkParams[5] * dh;

       v_u += Kepler::dudt(v_prevParams, acceleration) * dh;

       v_nu = v_u - v_kParameters.omega;

       double cosE = (std::cos(v_nu) + v_kParameters.e) / (1 + v_kParameters.e * std::cos(v_nu));

       v_r = v_kParameters.p * (1 - v_kParameters.e * cosE) / (1 - std::pow(keplerParameters.e, 2));
   }
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
