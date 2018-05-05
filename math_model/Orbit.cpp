#include "Orbit.h"

#include "Earth.h"
#include "Satellite.h"
#include "KeplerDifferentials.h"
#include "Forces.h"

#include <iostream>
//#include <geometric_object/solids/solids.h>

namespace math {
namespace model {
Orbit::Orbit(): phys::object(0) {};

Orbit & Orbit::setCentralMass(phys::object* centralMass) {
    if (m_centralMass != nullptr)
        m_mass -= m_centralMass->mass();
    m_centralMass = std::shared_ptr<phys::object>(centralMass);
    if (m_centralMass != nullptr)
        m_mass += m_centralMass->mass();
}

Orbit& Orbit::addPhysObject(const std::string& name, phys::object* physObject,
                            const helper::container::OrbitParameters& params) {
    if (physObject == nullptr)
        return *this;

    if (m_physObjects[name].first != nullptr)
        m_mass -= m_physObjects[name].first->mass();
    m_physObjects[name].first = std::shared_ptr<phys::object>(physObject);
    m_mass += m_physObjects[name].first->mass();

    // initialize satellite's track
    m_physObjectsTracks[name].clear();

    helper::container::OrbitParameters& parameters = m_physObjects[name].second;
    parameters = params;

    //initialize mu
    parameters.mu = helper::constant::G * (m_centralMass->mass() + physObject->mass());
    Orbit_DBGout << "mu = " << m_physObjects[name].second.mu << std::endl;

    double a = parameters.p / (1 - parameters.e * parameters.e);

    // initialize period T
    parameters.T = 2 * M_PI * std::sqrt(std::pow(a, 3) / parameters.mu);
    Orbit_DBGout << "T = " << parameters.T/3600 << " hour" << std::endl;

    double sclr_r;

    std::tie(sclr_r, parameters.nu) = helper::orbit::ellipticPolarCoordinates(parameters.t - parameters.time, parameters.p, parameters.e, parameters.mu);
    Orbit_DBGout << "\t|r(0)| = " << sclr_r << std::endl;
    Orbit_DBGout << "\tnu(0) = " << parameters.nu << std::endl;

    linear_algebra::Vector r = sclr_r * linear_algebra::Vector {std::cos(parameters.nu), std::sin(parameters.nu), 0, 1};
    r = mvp::action::rotate({0, 0, 1}, parameters.omega) *
        mvp::action::rotate({1, 0, 0}, parameters.i) *
        mvp::action::rotate({0, 0, 1}, parameters.Omega) *
        r;
    Orbit_DBGout << "\tr = " << r << std::endl;

    parameters.nu *= 180 / M_PI;

    m_physObjects[name].first->move(r);
    m_physObjects[name].first->rotate({0, 0, 1}, parameters.nu);
    m_physObjects[name].first->rotate({0, 0, 1}, parameters.Omega);
    m_physObjects[name].first->rotate({1, 0, 0}, parameters.i);
    m_physObjects[name].first->rotate({0, 0, 1}, parameters.omega);
}


Orbit& Orbit::addPhysObjectSail(const std::string& name, const helper::container::SailParameters& params) {
    m_physObjectsSails[name] = params;
    return *this;
}

Orbit& Orbit::removePhysObject(std::string& name) {
    if (m_physObjects[name].first != nullptr)
        m_mass -= m_physObjects[name].first->mass();
    m_physObjects.erase(name);
    m_physObjectsTracks.erase(name);
}

void Orbit::updateParameters(const std::string& name, double& r, double dt) {
    auto& obj = m_physObjects[name];
    auto& params = obj.second;

    helper::container::KeplerParameters keplerParameters;
    keplerParameters.omega = params.omega * M_PI / 180;
    keplerParameters.Omega = params.Omega * M_PI / 180;
    keplerParameters.e = params.e;
    keplerParameters.i = params.i * M_PI / 180;
    keplerParameters.mu = params.mu;
    keplerParameters.p = params.p;
    keplerParameters.nu = params.nu;
    keplerParameters.u = params.nu + params.omega * M_PI / 180;
    keplerParameters.r = r;
    double start_time = params.time;

    double dh = 0.1;
    size_t N = std::ceil(dt / dh);

    // initialize solar parameters
    double eps = 23 + 26./60;
    double lambda = 90;

    helper::container::SailParameters& sailParameters = m_physObjectsSails[name];

    for (size_t i = 1; i <= N; ++i) {
        // make solar normal vector
        linear_algebra::Vector sigma =
                mvp::action::rotate({1, 0, 0}, -keplerParameters.i / M_PI * 180) *
                mvp::action::rotate({0, 0, 1}, -keplerParameters.Omega / M_PI * 180) *
                mvp::action::rotate({1, 0, 0}, eps) *
                mvp::action::rotate({0, 0, 1}, lambda)
                * linear_algebra::Vector {1, 0, 0, 1};

        double theta_s = std::acos(sigma[0]);
        double psi_s = std::atan2(
                sigma[0] * std::cos(keplerParameters.u) + sigma[1] * std::sin(keplerParameters.u),
                sigma[0] * std::sin(keplerParameters.u) - sigma[1] * std::cos(keplerParameters.u)
        );

        linear_algebra::Vector solar_norm {
                std::sin(psi_s) * std::sin(theta_s),
                std::cos(psi_s) * std::sin(theta_s),
                std::cos(theta_s)
        };

        // compute current satellite orientation
        linear_algebra::Matrix v_orientation =
                mvp::action::rotate({0, 0, 1}, keplerParameters.Omega / M_PI * 180) *
                mvp::action::rotate({1, 0, 0}, keplerParameters.i / M_PI * 180) *
                mvp::action::rotate({0, 0, 1}, keplerParameters.omega / M_PI * 180) *
                mvp::action::rotate({0, 0, 1}, keplerParameters.nu / M_PI * 180); // check it!

        // reorient sail with current satellite orientation
        linear_algebra::Vector norm = sailParameters.norm;
        sailParameters.norm.resize(4, 1);
        sailParameters.norm = obj.first->orientation() * sailParameters.norm;
        sailParameters.norm.resize(3);

        // compute resulting force
        linear_algebra::Vector f =
                force::gravJ2(keplerParameters, obj.first->mass())
//                + force::atmos(keplerParameters, force::atm_density(r - helper::constant::EARTH_R), sailParameters.area, sailParameters.norm)
                + force::solar(sailParameters, solar_norm) * helper::orbit::shadow(keplerParameters, solar_norm, sailParameters)
        ;
        f /= obj.first->mass();

        sailParameters.norm = norm;

        Orbit_DBGout << "original force: " << f << std::endl;

        // reorient resulting force
        f.resize(4, 1);
        f = v_orientation * f;
        Orbit_DBGout << "reoriented force: " << f << std::endl;

        // update Kepler's orbital elements
        auto v_prevParams = keplerParameters;
        keplerParameters.omega += Kepler::domegadt(v_prevParams, f) * dh;
        keplerParameters.Omega += Kepler::dOmegadt(v_prevParams, f) * dh;
        keplerParameters.p += Kepler::dpdt(v_prevParams, f) * dh;
        keplerParameters.e += Kepler::dedt(v_prevParams, f) * dh;
        keplerParameters.i += Kepler::didt(v_prevParams, f) * dh;
        keplerParameters.u += Kepler::dudt(v_prevParams, f) * dh;
        start_time += Kepler::dtaudt(v_prevParams, f) * dh;
        keplerParameters.nu = keplerParameters.u - keplerParameters.omega;

        double cosE = (std::cos(keplerParameters.nu) + keplerParameters.e) / (1 + keplerParameters.e * std::cos(keplerParameters.nu));

        keplerParameters.r = keplerParameters.p * (1 - keplerParameters.e * cosE) / (1 - std::pow(keplerParameters.e, 2));
    }
    Orbit_DBGout << "omega: " << keplerParameters.omega << std::endl;
    Orbit_DBGout << "Omega: " << keplerParameters.Omega << std::endl;
    Orbit_DBGout << "p: " << keplerParameters.p << std::endl;
    Orbit_DBGout << "e: " << keplerParameters.e << std::endl;
    Orbit_DBGout << "a: " << keplerParameters.p / (1 - keplerParameters.e * keplerParameters.e) << std::endl;
    Orbit_DBGout << "i: " << keplerParameters.i << std::endl;
    Orbit_DBGout << "nu: " << keplerParameters.nu << std::endl;
    Orbit_DBGout << "time: " << start_time << std::endl;


    params.omega = keplerParameters.omega / M_PI * 180;
    params.Omega = keplerParameters.Omega / M_PI * 180;
    params.e = keplerParameters.e;
    params.i = keplerParameters.i / M_PI * 180;
    params.p = keplerParameters.p;
    params.nu = keplerParameters.u - keplerParameters.omega;

    double cosE = (std::cos(keplerParameters.nu) + keplerParameters.e) / (1 + keplerParameters.e * std::cos(keplerParameters.nu));
    double sinE = std::sin(keplerParameters.nu) * std::sqrt(1 - std::pow(keplerParameters.e, 2));
    params.E = std::atan2(sinE, cosE);

    r = keplerParameters.r;
//    params.time = start_time;
}

Orbit& Orbit::update(double dt /*sec*/) {
    for (auto& pr : m_physObjects) {
        Orbit_DBGout << "name: " << pr.first << std::endl;
        auto& obj = pr.second;

        // link satellite's track
        auto& track = m_physObjectsTracks[pr.first];

        obj.second.t = (obj.second.t + dt) - (int)((obj.second.t + dt) / obj.second.T) * obj.second.T;
        Orbit_DBGout << "\ttime = " << obj.second.t << " (sec)" << std::endl;

        double sclr_r;

        std::tie(sclr_r, obj.second.nu) = helper::orbit::ellipticPolarCoordinates(obj.second.t - obj.second.time, obj.second.p, obj.second.e, obj.second.mu);
        obj.second.E = helper::orbit::anomaly(obj.second.t - obj.second.time, obj.second.p, obj.second.e, obj.second.mu);
        Orbit_DBGout << "\t|r(0)| = " << sclr_r << std::endl;
        Orbit_DBGout << "\tnu(0) = " << obj.second.nu << std::endl;

        updateParameters(pr.first, sclr_r, dt);

        linear_algebra::Vector r = sclr_r * linear_algebra::Vector {std::cos(obj.second.nu), std::sin(obj.second.nu), 0, 1};
        r = mvp::action::rotate({0, 0, 1}, obj.second.Omega) *
            mvp::action::rotate({1, 0, 0}, obj.second.i) *
            mvp::action::rotate({0, 0, 1}, obj.second.omega) *
            r;
        Orbit_DBGout << "\tr = " << r << std::endl;

        obj.second.nu *= 180 / M_PI;
        obj.second.E *= 180 / M_PI;

        obj.first->move(r-obj.first->position());
        obj.first->orientation(
                mvp::action::rotate({0, 0, 1}, obj.second.Omega) *
                mvp::action::rotate({1, 0, 0}, obj.second.i) *
                mvp::action::rotate({0, 0, 1}, obj.second.omega) *
                mvp::action::rotate({0, 0, 1}, obj.second.E)
        );
        OrbitLOG_out << r << std::endl;

        // update satellite's track
        if (track.size() >= TRACK_MAX_SIZE)
            track.pop_front();
        track.push_back(obj.first->position());
    }
}

Orbit& Orbit::render(std::list<std::shared_ptr<glsl::object>>& draw_list) {
    draw_list.clear();
    double scalingFactor = helper::constant::EARTH_R / 2;
    auto* center = new shape::solid::sphere(32, 32, 1);
    center->move(linear_algebra::Vector {
                    m_centralMass->position()[0], // x
                    m_centralMass->position()[1], // y
                    m_centralMass->position()[2]  // z
            })
            .scale(linear_algebra::Vector {
                    m_centralMass->scale()[0][0], // x
                    m_centralMass->scale()[1][1], // y
                    m_centralMass->scale()[2][2]  // z
            } / scalingFactor / 2
            );
    center->orientation(m_centralMass->orientation());
    center->update_color(helper::color(150, 50, 50));
    draw_list.push_back(std::shared_ptr<glsl::object>(center));

    for (auto& pr : m_physObjects) {
        auto& obj = pr.second.first;
        glsl::object* draw_obj = new shape::solid::sphere(32, 32, 0.1);
        draw_obj->move(linear_algebra::Vector {
                    obj->position()[0], // x
                    obj->position()[1], // y
                    obj->position()[2]  // z
            } / scalingFactor);
        draw_obj->orientation(obj->orientation());
        draw_obj->update_color(helper::color(100, 100, 100));
        draw_list.push_back(std::shared_ptr<glsl::object>(draw_obj));

        // link satellite's track
        auto& track = m_physObjectsTracks[pr.first];

        auto it = track.begin();
        if (it != track.end()) {
            linear_algebra::Vector v_start = *it;
            size_t counter = 0;
            ++it;
            while (it != track.end()) {
                linear_algebra::Vector v_end = *it;
                draw_obj = new shape::line(linear_algebra::Vector {0, 0, 0, 1}, (v_end - v_start) / scalingFactor);
                draw_obj->move(v_start / scalingFactor);
                draw_obj->update_color(helper::color((50 + counter*3/2) % 256, (70 + counter*2/3) % 256, (120 + counter/3) % 256));
                draw_list.push_back(std::shared_ptr<glsl::object>(draw_obj));
                v_start = v_end;
                ++counter;
                ++it;
            }
        }
    }
}
}
}