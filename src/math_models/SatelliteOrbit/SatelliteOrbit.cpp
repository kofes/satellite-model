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

    return *this;
}

SatelliteOrbit& SatelliteOrbit::parameters(const helper::container::KeplerParameters& params) {
    m_keplerParmeters = params;

    // initialize satellite's track
    if (m_save_track)
        m_track.clear();

    return *this;
}

SatelliteOrbit& SatelliteOrbit::update(double dt /*sec*/, bool maximization) {
    m_time += dt;

    double E = helper::orbit::E(m_keplerParmeters, m_Earth.mass(), m_satellite.mass(), m_time);
    double nu = helper::orbit::nu(m_keplerParmeters, E);
    double scalarR = helper::orbit::r(m_keplerParmeters, E);

    updateParameters(scalarR, nu, dt, 1, maximization);

    linear_algebra::Vector r = helper::orbit::r(scalarR, nu);

    r = helper::orbit::r(m_keplerParmeters, r);

    // std::cout << "r: " << r << std::endl;

    m_satellite.position(r);

    // update satellite's track
    if (m_save_track) {
        // remove last point & add new
        if (m_track.size() >= TRACK_MAX_COUNT_POINTS)
            m_track.pop_front();
        m_track.emplace_back(m_satellite.position());
    }

    return *this;
}

void SatelliteOrbit::updateParameters(double& sclrR, double& nu, double dt, double dh, bool maximization) {
    // initialize solar parameters
    double tmp_lambda = 40;

    // FIND OUT OPTIMAL ANGLES
    linear_algebra::Vector newAngles = helper::optimization::gradient([&] (const linear_algebra::Vector& val_angles) -> double {
        linear_algebra::Vector vec = helper::integral::rk4(
            [&](const linear_algebra::Vector& val, double t) {
                // copy parameters to container
                helper::container::KeplerParameters tmp_parameters;
                tmp_parameters.p = val[0];
                tmp_parameters.e = val[1];
                tmp_parameters.omega = val[2];
                tmp_parameters.i = val[3];
                tmp_parameters.Omega = val[4];
                tmp_parameters.tau = val[5];
                double tmp_u = val[6];

                // make solar normal vector !at global orientation system!
                linear_algebra::Vector sigma =
                        mvp::action::R_x(-tmp_parameters.i) *
                        mvp::action::R_z(-tmp_parameters.Omega) *
                        mvp::action::R_x(helper::constant::EARTH_ECLIPTIC) *
                        mvp::action::R_z(tmp_lambda)
                        * linear_algebra::Vector {1, 0, 0, 1};

                double theta_s = std::acos(sigma[0]);
                double psi_s = std::atan2(
                        sigma[0] * std::cos(tmp_u) + sigma[1] * std::sin(tmp_u),
                        sigma[0] * std::sin(tmp_u) - sigma[1] * std::cos(tmp_u)
                );

                linear_algebra::Vector solar_norm {
                        std::sin(psi_s) * std::sin(theta_s),
                        std::cos(psi_s) * std::sin(theta_s),
                        std::cos(theta_s)
                };

                // intialize main orientation system
                linear_algebra::Matrix orbital_orientation =
                            mvp::action::R_z(tmp_parameters.Omega) *
                            mvp::action::R_x(tmp_parameters.i) *
                            mvp::action::R_z(tmp_parameters.omega) *
                            mvp::action::R_z(tmp_u);

                // reorient sail norm to orbital orientation system
                auto sail_norm = m_satellite.sailParameters().norm;

                orbital_orientation.resize(3, 0);
                sail_norm.resize(3);

                sail_norm = orbital_orientation * sail_norm; // to orbital orientation system

                // SET NEW ANGLES!
                auto angles = m_satellite.angles();

                // IMPORTANT: PUTTING NEW ANGLES VALUES HERE
                double deltaT = (t - m_time + dt);
                angles += val_angles * deltaT;

                linear_algebra::Matrix satellite_orientation =
                            mvp::action::R_x(angles[2]) * // gamma
                            mvp::action::R_z(angles[1]) * // beta
                            mvp::action::R_y(angles[0]);  // alpha
                // reorient sail norm to global orientation system
                satellite_orientation.resize(3, 0);
                auto params = m_satellite.sailParameters();
                params.norm = satellite_orientation * sail_norm; // to global orientation system

                double tmp_mu = helper::constant::G * (m_Earth.mass() + m_satellite.mass());
                double tmp_E = helper::orbit::E(tmp_parameters, m_Earth.mass(), m_satellite.mass(), t);
                double tmp_r = helper::orbit::r(tmp_parameters, tmp_E);
                double tmp_nu = helper::orbit::nu(tmp_parameters, tmp_E);

                auto force =
                            // + force::gravJ2(tmp_parameters, m_satellite.mass(), tmp_mu, tmp_r, tmp_nu)
                            + force::solar(params, solar_norm)
                            // + force::atmos(tmp_parameters, force::atm_density(tmp_r - m_Earth.R()), params.area, params.norm, tmp_mu, tmp_r, tmp_nu)
                           ;
                 //

                auto tmp_acceleration = force / m_satellite.mass();
                // update Kepler's orbital elements
                auto tmp_newParams = kepler::differentials::parameters(
                    tmp_parameters,
                    m_Earth.mass(),
                    m_satellite.mass(),
                    t,
                    tmp_acceleration
                );

                double tmp_dU = kepler::differentials::dudt(
                    tmp_r,
                    tmp_mu,
                    tmp_nu,
                    tmp_parameters,
                    tmp_acceleration);

                return linear_algebra::Vector {
                    tmp_newParams.p,                  // 0
                    tmp_newParams.e,                  // 1
                    tmp_newParams.omega, // 2
                    tmp_newParams.i,     // 3
                    tmp_newParams.Omega, // 4
                    tmp_newParams.tau,                // 5
                    tmp_dU                             // 6
                };
            },
            linear_algebra::Vector {
                m_keplerParmeters.p,                      // 0
                m_keplerParmeters.e,                      // 1
                m_keplerParmeters.omega * M_PI / 180,     // 2
                m_keplerParmeters.i * M_PI / 180,         // 3
                m_keplerParmeters.Omega * M_PI / 180,     // 4
                m_keplerParmeters.tau,                    // 5
                nu + m_keplerParmeters.omega * M_PI / 180 // 6
            },
            m_time - dt,
            m_time,
            dh
        );
        return (vec[0] + 100 * vec[1]); // return p, e
    },
    maximization,
    3,
    linear_algebra::Vector(3, -M_PI / 180 * dh),
    linear_algebra::Vector(3, M_PI / 180 * dh),
    0.01,
    1e-4,
    50
    );
    // RECOMPUTE Kepler Parameters
    linear_algebra::Vector result = helper::integral::rk4(
        [&](const linear_algebra::Vector& val, double t) {
            // copy parameters to container
            helper::container::KeplerParameters tmp_parameters;
            tmp_parameters.p = val[0];
            tmp_parameters.e = val[1];
            tmp_parameters.omega = val[2];
            tmp_parameters.i = val[3];
            tmp_parameters.Omega = val[4];
            tmp_parameters.tau = val[5];
            double tmp_u = val[6];

            // make solar normal vector !at global orientation system!
            linear_algebra::Vector sigma =
                    mvp::action::R_x(-tmp_parameters.i) *
                    mvp::action::R_z(-tmp_parameters.Omega) *
                    mvp::action::R_x(helper::constant::EARTH_ECLIPTIC) *
                    mvp::action::R_z(tmp_lambda)
                    * linear_algebra::Vector {1, 0, 0, 1};

            double theta_s = std::acos(sigma[0]);
            double psi_s = std::atan2(
                    sigma[0] * std::cos(tmp_u) + sigma[1] * std::sin(tmp_u),
                    sigma[0] * std::sin(tmp_u) - sigma[1] * std::cos(tmp_u)
            );

            linear_algebra::Vector solar_norm {
                    std::sin(psi_s) * std::sin(theta_s),
                    std::cos(psi_s) * std::sin(theta_s),
                    std::cos(theta_s)
            };

            // intialize main orientation system
            linear_algebra::Matrix orbital_orientation =
                        mvp::action::R_z(tmp_parameters.Omega) *
                        mvp::action::R_x(tmp_parameters.i) *
                        mvp::action::R_z(tmp_parameters.omega) *
                        mvp::action::R_z(tmp_u);

            // reorient sail norm to orbital orientation system
            auto sail_norm = m_satellite.sailParameters().norm;

            orbital_orientation.resize(3, 0);
            sail_norm.resize(3);

            sail_norm = orbital_orientation * sail_norm; // to orbital orientation system

            auto angles = m_satellite.angles();
            angles += newAngles * (t - m_time + dt);

            linear_algebra::Matrix satellite_orientation =
                        mvp::action::R_x(angles[2]) * // gamma
                        mvp::action::R_z(angles[1]) * // beta
                        mvp::action::R_y(angles[0]);  // alpha
            // reorient sail norm to global orientation system
            satellite_orientation.resize(3, 0);
            auto params = m_satellite.sailParameters();
            params.norm = satellite_orientation * sail_norm; // to global orientation system

            double tmp_mu = helper::constant::G * (m_Earth.mass() + m_satellite.mass());
            double tmp_E = helper::orbit::E(tmp_parameters, m_Earth.mass(), m_satellite.mass(), t);
            double tmp_r = helper::orbit::r(tmp_parameters, tmp_E);
            double tmp_nu = helper::orbit::nu(tmp_parameters, tmp_E);

            auto force =
                        // + force::gravJ2(tmp_parameters, m_satellite.mass(), tmp_mu, tmp_r, tmp_nu)
                        + force::solar(params, solar_norm)
                        // + force::atmos(tmp_parameters, force::atm_density(tmp_r - m_Earth.R()), params.area, params.norm, tmp_mu, tmp_r, tmp_nu)
                       ;
             //

            auto tmp_acceleration = force / m_satellite.mass();
            // update Kepler's orbital elements
            auto tmp_newParams = kepler::differentials::parameters(
                tmp_parameters,
                m_Earth.mass(),
                m_satellite.mass(),
                t,
                tmp_acceleration
            );

            double tmp_dU = kepler::differentials::dudt(
                tmp_r,
                tmp_mu,
                tmp_nu,
                tmp_parameters,
                tmp_acceleration);

            return linear_algebra::Vector {
                tmp_newParams.p,                  // 0
                tmp_newParams.e,                  // 1
                tmp_newParams.omega, // 2
                tmp_newParams.i,     // 3
                tmp_newParams.Omega, // 4
                tmp_newParams.tau,                // 5
                tmp_dU                             // 6
            };
        },
        linear_algebra::Vector {
            m_keplerParmeters.p,                      // 0
            m_keplerParmeters.e,                      // 1
            m_keplerParmeters.omega * M_PI / 180,     // 2
            m_keplerParmeters.i * M_PI / 180,         // 3
            m_keplerParmeters.Omega * M_PI / 180,     // 4
            m_keplerParmeters.tau,                    // 5
            nu + m_keplerParmeters.omega * M_PI / 180 // 6
        },
        m_time - dt,
        m_time,
        dh
    );

    // UPDATE SATELLITE ANGLES
    m_satellite.angles(m_satellite.angles() + newAngles * dt);

   double v_u = nu + m_keplerParmeters.omega * M_PI / 180;

   // write result:
   m_keplerParmeters.p += result[0];
   m_keplerParmeters.e += result[1];
   m_keplerParmeters.omega += result[2] / M_PI * 180;
   m_keplerParmeters.i += result[3] / M_PI * 180;
   m_keplerParmeters.Omega += result[4] / M_PI * 180;
   // m_keplerParmeters.tau += result[5];
   v_u += result[6];

   double v_E = helper::orbit::E(m_keplerParmeters, m_Earth.mass(), m_satellite.mass(), m_time);
   double v_r = helper::orbit::r(m_keplerParmeters, v_E);

    sclrR = v_r;
    nu = v_u - m_keplerParmeters.omega * M_PI / 180;
}

math::model::Satellite SatelliteOrbit::satellite() {
    return m_satellite;
}

helper::container::KeplerParameters SatelliteOrbit::parameters() {
    return m_keplerParmeters;
}

SatelliteOrbit& SatelliteOrbit::render(std::list<std::shared_ptr<glsl::object>>& result) {
    result.clear();
    double scalingFactor = helper::constant::EARTH_R / 2;

    // render center object
    auto* center = new shape::solid::sphere(32, 32, 1);
    center->move(linear_algebra::Vector {
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
    center->orientation(m_Earth.orientation());
    center->update_color(helper::color(150, 50, 50));
    result.push_back(std::shared_ptr<glsl::object>(center));

    // render orbit orbital_orientation
    // intialize main orientation system
    double v_E = helper::orbit::E(m_keplerParmeters, m_Earth.mass(), m_satellite.mass(), m_time);
    linear_algebra::Matrix orbital_orientation =
            mvp::action::rotate({0, 0, 1}, m_keplerParmeters.Omega) *
            mvp::action::rotate({1, 0, 0}, m_keplerParmeters.i) *
            mvp::action::rotate({0, 0, 1}, m_keplerParmeters.omega) *
            mvp::action::rotate({0, 0, 1}, v_E * 180 / M_PI);
    glsl::object* draw_orbitOrientation = new shape::solid::sphere(32, 32, 0.1);
    // std::cout << "position: " << m_satellite.position() << std::endl;
    draw_orbitOrientation->move(m_satellite.position() / scalingFactor);
    draw_orbitOrientation->orientation(orbital_orientation);
    draw_orbitOrientation->update_color(helper::color(100, 100, 100));
    result.push_back(std::shared_ptr<glsl::object>(draw_orbitOrientation));


    // render satellite object
    glsl::object* draw_satellite = new shape::solid::sphere(32, 32, 0.1);
    // std::cout << "position: " << m_satellite.position() << std::endl;
    draw_satellite->move(m_satellite.position() / scalingFactor);
    draw_satellite->orientation(orbital_orientation * m_satellite.orientation());
    draw_satellite->update_color(helper::color(100, 100, 100));
    result.push_back(std::shared_ptr<glsl::object>(draw_satellite));

    // render track
    if (m_save_track) {
        auto it = m_track.begin();
        if (it != m_track.end()) {
            linear_algebra::Vector v_start = *it;
            size_t counter = 0;
            ++it;
            while (it != m_track.end()) {
                linear_algebra::Vector v_end = *it;
                glsl::object* draw_track = new shape::line(linear_algebra::Vector {0, 0, 0, 1}, (v_end - v_start) / scalingFactor);
                draw_track->move(v_start / scalingFactor);
                draw_track->update_color(helper::color(
                    (50 + int(counter * 200. / TRACK_MAX_COUNT_POINTS)) % 256,
                    (70 + int(counter * 150. / TRACK_MAX_COUNT_POINTS)) % 256,
                    (120 + int(counter * 70. / TRACK_MAX_COUNT_POINTS)) % 256
                ));
                result.push_back(std::shared_ptr<glsl::object>(draw_track));
                v_start = v_end;
                ++counter;
                ++it;
            }
        }
    }

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
