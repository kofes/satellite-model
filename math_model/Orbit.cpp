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

Orbit& Orbit::removePhysObject(std::string& name) {
    if (m_physObjects[name].first != nullptr)
        m_mass -= m_physObjects[name].first->mass();
    m_physObjects.erase(name);
    m_physObjectsTracks.erase(name);
}

void Orbit::updateParameters(helper::container::OrbitParameters& params, double mass, double& r, double dt) {
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

    linear_algebra::Vector solar_norm {-1, 0, 0};

    helper::container::SailParameters sailParameters;

    sailParameters.rho = 0.9;
    sailParameters.Bf = sailParameters.Bb = 2./3;
    sailParameters.s = 0.9;
    sailParameters.ef = 2;
    sailParameters.eb = 1.9;
    sailParameters.norm = linear_algebra::Vector {1, 0, 0};
    sailParameters.area = 400;

    for (size_t i = 1; i <= N; ++i) {
        linear_algebra::Vector f =
                force::gravJ2(keplerParameters, mass)
//                + force::atmos(keplerParameters, force::atm_density(r - helper::constant::EARTH_R), sail_area, sail_norm)
//                + force::solar(sailParameters, solar_norm)
        ;
        Orbit_DBGout << "force: " << f << std::endl;
//        std::cout << "gravity force: " << force1 << std::endl;
//        std::cout << "atmosphere force: " << force2 << std::endl;
//        std::cout << "solar force: " << force3 << std::endl;


        std::tie(r, keplerParameters.nu) = helper::orbit::ellipticPolarCoordinates(
                params.t - params.time - dt + dh * i,
                keplerParameters.p, keplerParameters.e, params.mu);

        keplerParameters.omega += Kepler::domegadt(keplerParameters, f) * dh;
        keplerParameters.Omega += Kepler::dOmegadt(keplerParameters, f) * dh;
        keplerParameters.p += Kepler::dpdt(keplerParameters, f) * dh;
        keplerParameters.e += Kepler::dedt(keplerParameters, f) * dh;
        keplerParameters.i += Kepler::didt(keplerParameters, f) * dh;
        start_time += Kepler::dtaudt(keplerParameters, f) * dh;

        keplerParameters.u = (keplerParameters.nu + keplerParameters.omega);

        start_time += Kepler::dtaudt(keplerParameters, f) * dh;
    }
    Orbit_DBGout << "omega: " << keplerParameters.omega << std::endl;
    Orbit_DBGout << "Omega: " << keplerParameters.Omega << std::endl;
    Orbit_DBGout << "p: " << keplerParameters.p << std::endl;
    Orbit_DBGout << "e: " << keplerParameters.e << std::endl;
    Orbit_DBGout << "a: " << keplerParameters.p / (1 - keplerParameters.e * keplerParameters.e) << std::endl;
    Orbit_DBGout << "i: " << keplerParameters.i << std::endl;
    Orbit_DBGout << "nu: " << keplerParameters.nu << std::endl;
    Orbit_DBGout << "time: " << start_time << std::endl;


//    params.omega = keplerParameters.omega / M_PI * 180;
//    params.Omega = keplerParameters.Omega / M_PI * 180;
//    params.e = keplerParameters.e;
//    params.i = keplerParameters.i / M_PI * 180;
//    params.p = keplerParameters.p;
//    params.nu = keplerParameters.nu;
//    params.u = keplerParameters.nu + params.omega;
//    keplerParameters.r = r;
//    params.time = start_time;

    double a = params.p / (1 - params.e * params.e);

    double n = std::sqrt(params.mu / std::pow(a, 3));
    double tau = dt * n;

    params.omega += 3./2 * helper::constant::J2 * std::pow(helper::constant::EARTH_R / params.p, 2) * std::cos(params.i) * tau / M_PI * 180;
    params.Omega += 3./4 * helper::constant::J2 * std::pow(helper::constant::EARTH_R / params.p, 2) * (1 - 5*std::pow(std::cos(params.i), 2)) * tau / M_PI * 180;
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
        Orbit_DBGout << "\t|r(0)| = " << sclr_r << std::endl;
        Orbit_DBGout << "\tnu(0) = " << obj.second.nu << std::endl;

        Orbit_DBGout << "\tOmega = " << obj.second.Omega << std::endl;
        Orbit_DBGout << "\tomega = " << obj.second.omega << std::endl;
        updateParameters(obj.second, obj.first->mass(), sclr_r, dt);
        Orbit_DBGout << "\tOmega = " << obj.second.Omega << std::endl;
        Orbit_DBGout << "\tomega = " << obj.second.omega << std::endl;

        linear_algebra::Vector r = sclr_r * linear_algebra::Vector {std::cos(obj.second.nu), std::sin(obj.second.nu), 0, 1};
        r = mvp::action::rotate({0, 0, 1}, obj.second.Omega) *
            mvp::action::rotate({1, 0, 0}, obj.second.i) *
            mvp::action::rotate({0, 0, 1}, obj.second.omega) *
            r;
        Orbit_DBGout << "\tr = " << r << std::endl;

        obj.second.nu *= 180 / M_PI;

        obj.first->move(r-obj.first->position());
        obj.first->orientation(
                mvp::action::rotate({0, 0, 1}, obj.second.Omega) *
                mvp::action::rotate({1, 0, 0}, obj.second.i) *
                mvp::action::rotate({0, 0, 1}, obj.second.omega) *
                mvp::action::rotate({0, 0, 1}, obj.second.nu)
        );
        OrbitLOG_out << r << std::endl;

        // update satellite's track
        if (track.size() >= TRACK_MAX_SIZE)
            track.pop_front();
        track.push_back(r);
    }
}

Orbit& Orbit::render(std::list<std::shared_ptr<glsl::object>>& draw_list) {
    draw_list.clear();
    double EARTH_R = 6371e+3;
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
            } / EARTH_R
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
            } / EARTH_R * 2);
        draw_obj->orientation(obj->orientation());
        draw_obj->update_color(helper::color(100, 100, 100));
        draw_list.push_back(std::shared_ptr<glsl::object>(draw_obj));

        // link satellite's track
        auto& track = m_physObjectsTracks[pr.first];

        auto it = track.begin();
        if (it != track.end()) {
            linear_algebra::Vector v_start = *it;
            size_t counter = 0;
            while (++it != track.end()) {
                linear_algebra::Vector v_end = *it;
                draw_obj = new shape::line(v_start / EARTH_R, v_end / EARTH_R);
                draw_obj->update_color(helper::color((50 + counter/2) % 256, (70 + counter*2) % 256, (120 + counter) % 256));
                draw_list.push_back(std::shared_ptr<glsl::object>(draw_obj));
                v_start = v_end;
                ++counter;
            }
        }
    }
}
}
}