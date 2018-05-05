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
                            const linear_algebra::Vector& satellite_start_point,
                            const linear_algebra::Vector& satellite_start_speed) {
    if (physObject == nullptr)
        return *this;

    if (m_physObjects[name].first != nullptr)
        m_mass -= m_physObjects[name].first->mass();
    m_physObjects[name].first = std::shared_ptr<phys::object>(physObject);
    m_mass += m_physObjects[name].first->mass();

    // initialize start point
    m_physObjects[name].first->move(satellite_start_point);

    OrbitParameters& parameters = m_physObjects[name].second;
    //initialize mu
    parameters.mu = helper::constant::G * (m_centralMass->mass() + physObject->mass());
    Orbit_DBGout << "mu = " << m_physObjects[name].second.mu << std::endl;

    double distance = satellite_start_point.length();
    Orbit_DBGout << "distance = " << distance  << " m" << std::endl;

    double circle_speed = std::sqrt(helper::constant::G * m_centralMass->mass() / distance);
    Orbit_DBGout << "circle speed = " << circle_speed << " m/sec" << std::endl;

    double start_speed = satellite_start_speed.length();
    Orbit_DBGout << "start speed = " << start_speed << " m/sec" << std::endl;

    double a = 1 / (
            2 / distance -
            satellite_start_speed * satellite_start_speed / (helper::constant::G * m_centralMass->mass())
    );
    Orbit_DBGout << "a = " << a << " m" << std::endl;


    /**
     * if start_speed >  circle_speed, then 'perigei' at start_point
     * if start_speed == circle_speed, then we have circle!!!
     * if start_speed <  circle_speed, then 'apogei' at start_point
     */

    // initialize normal
    parameters.norm = linear_algebra::vectorMultiply({satellite_start_point, satellite_start_speed}).normalize();
    Orbit_DBGout << "norm = " << parameters.norm << std::endl;

    // initialize e_r
    parameters.e_r = satellite_start_point.normalize();
    Orbit_DBGout << "e_r = " << parameters.e_r << std::endl;

    // initialize e_t
    parameters.e_tau = (start_speed > circle_speed ? 1 : -1) * satellite_start_speed.normalize();
    Orbit_DBGout << "e_t = " << parameters.e_tau << std::endl;

    // initialize i
    parameters.i = std::acos(parameters.norm * linear_algebra::Vector {0, 0, 1});
    Orbit_DBGout << "i = " << parameters.i << std::endl;

    double r_1 = 2 * a - distance;

    // initialize p
    parameters.p = 2 * r_1 * distance / (r_1 + distance);
    Orbit_DBGout << "p = " << parameters.p << " m" << std::endl;

    // initialize e
    parameters.e = std::fabs(r_1 - distance) / (r_1 + distance);
    Orbit_DBGout << "e = " << parameters.e << std::endl;

    // initialize period T
    parameters.T = 2 * M_PI * std::sqrt(a * a * a / parameters.mu);
    Orbit_DBGout << "T = " << parameters.T/3600 << " hour" << std::endl;

    // initialize perigei time
    parameters.time = parameters.T / 2 * (start_speed > circle_speed);
    Orbit_DBGout << "time = " << parameters.time/3600 << " hour" << std::endl;

    // initialize time
    parameters.t = 0;
    Orbit_DBGout << "time = " << parameters.t/3600 << " hour" << std::endl;

    // initialize start nu(t_0)
    parameters.nu = M_PI * (start_speed < circle_speed);
    Orbit_DBGout << "nu = " << parameters.nu << " rad" << std::endl;

    // initialize rotation

    m_physObjects[name].first->orientation(
            linear_algebra::Matrix {
                    {  parameters.e_r[0],   parameters.e_r[1],   parameters.e_r[2], 0},
                    {parameters.e_tau[0], parameters.e_tau[1], parameters.e_tau[2], 0},
                    { parameters.norm[0],  parameters.norm[1],  parameters.norm[2], 0},
                    {                 0 ,                  0 ,                  0 , 1}
            });
}

Orbit& Orbit::addPhysObject(const std::string& name, phys::object* physObject,
                            const OrbitParameters& params) {
    if (physObject == nullptr)
        return *this;

    if (m_physObjects[name].first != nullptr)
        m_mass -= m_physObjects[name].first->mass();
    m_physObjects[name].first = std::shared_ptr<phys::object>(physObject);
    m_mass += m_physObjects[name].first->mass();

    OrbitParameters& parameters = m_physObjects[name].second;
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
}

Orbit& Orbit::update(double dt /*sec*/) {
    for (auto& pr : m_physObjects) {
        Orbit_DBGout << "name: " << pr.first << std::endl;
        auto& obj = pr.second;
        auto r = obj.first->position();
        Orbit_DBGout << "\tr = " << r << " (m)" << std::endl;

        obj.second.t = (obj.second.t + dt) - (int)((obj.second.t + dt) / obj.second.T) * obj.second.T;
        Orbit_DBGout << "\ttime = " << obj.second.t << " (sec)" << std::endl;

        double E = helper::orbit::anomaly(obj.second.t - obj.second.time, obj.second.p, obj.second.e, obj.second.mu);

        double a = obj.second.p / (1 - obj.second.e * obj.second.e);
        double tmp_E = 1 - obj.second.e * std::cos(E);

        double sclr_r = a * tmp_E;
        Orbit_DBGout << "\tr(E) = " << sclr_r << std::endl;

        double c_nu = (std::cos(E) - obj.second.e)/tmp_E;
        Orbit_DBGout << "\tcos(nu(E)) = " << c_nu << std::endl;

        double s_nu = std::sin(E)/tmp_E * std::sqrt(1 - obj.second.e * obj.second.e);
        Orbit_DBGout << "\tsin(nu(E)) = " << s_nu << std::endl;
        Orbit_DBGout << "\t1 = " << s_nu*s_nu + c_nu * c_nu << std::endl;

        linear_algebra::Vector dr =
                linear_algebra::Matrix {
                        {  obj.second.e_r[0],   obj.second.e_r[1],   obj.second.e_r[2], 0},
                        {obj.second.e_tau[0], obj.second.e_tau[1], obj.second.e_tau[2], 0},
                        { obj.second.norm[0],  obj.second.norm[1],  obj.second.norm[2], 0},
                        {                 0 ,                  0 ,                  0 , 1}
                } * linear_algebra::Vector {
                        sclr_r *c_nu,
                        sclr_r *s_nu, 0, 1} - r;
        Orbit_DBGout << "\tdr = " << dr << std::endl;
        obj.first->move(dr);
        Orbit_DBGout << "\tsatellite pos = " << obj.first->position() << " (m)" << std::endl;
        {
            linear_algebra::Vector vec1 {r[0], r[1], r[2]};
            linear_algebra::Vector vec2 {obj.first->position()[0], obj.first->position()[1], obj.first->position()[2]};
            linear_algebra::Vector e_r = vec1 / vec1.length();
            linear_algebra::Vector e_dr = vec2 / vec2.length();
            double c_angle = e_dr * e_r;
            double s_angle = linear_algebra::vectorMultiply({e_dr, e_r}).length();
            Orbit_DBGout << "\te_r = " << e_r << std::endl;
            Orbit_DBGout << "\te_dr = " << e_dr << std::endl;
            obj.first->rotate(obj.second.norm, std::atan2(s_angle, c_angle) * 180 / M_PI);
            Orbit_DBGout << "\tangle = " << std::atan2(s_angle, c_angle) * 180 / M_PI << std::endl;
            Orbit_DBGout << "\tOrientation = " << obj.first->orientation() << std::endl;
        }
    }
}

void Orbit::updateParameters(OrbitParameters& params, double mass, double& r, double dt) {
    Kepler::Parameters keplerParameters;
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

    double sail_area = 400;
    linear_algebra::Vector sail_norm {1, 0, 0};
    linear_algebra::Vector solar_norm {-1, 0, 0};

    force::SailParameters sailParameters;

    sailParameters.rho = 0.9;
    sailParameters.Bf = sailParameters.Bb = 2./3;
    sailParameters.s = 0.9;
    sailParameters.ef = 2;
    sailParameters.eb = 1.9;

    for (size_t i = 1; i <= N; ++i) {
        linear_algebra::Vector f =
                force::gravJ2(keplerParameters, mass)
//                + force::atmos(keplerParameters, force::atm_density(r - helper::constant::EARTH_R), sail_area, sail_norm)
//                + force::solar(sailParameters, sail_area, solar_norm, sail_norm)
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

Orbit& Orbit::updateKepler(double dt) {
//    m_centralMass->rotate({0, 1, 0}, 5);
    for (auto& pr : m_physObjects) {
        Orbit_DBGout << "name: " << pr.first << std::endl;
        auto& obj = pr.second;

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
        auto* draw_obj = new shape::solid::sphere(32, 32, 0.1);
        draw_obj->move(linear_algebra::Vector {
                    obj->position()[0], // x
                    obj->position()[1], // y
                    obj->position()[2]  // z
            } / EARTH_R * 2);
//        draw_obj->scale(linear_algebra::Vector {
//                0.5, 0.5, 0.5
//            });
        draw_obj->orientation(obj->orientation());
        draw_obj->update_color(helper::color(100, 100, 100));
        draw_list.push_back(std::shared_ptr<glsl::object>(draw_obj));
    }
}
}
}