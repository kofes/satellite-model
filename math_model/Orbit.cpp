#include "Orbit.h"

#include "Earth.h"
#include "Satellite.h"

#include <iostream>
#include <geometric_object/solids/solids.h>

namespace math {
namespace model {
Orbit::Orbit() = default;

Orbit & Orbit::setCentralMass(phys::object* centralMass) {
    m_centralMass = std::shared_ptr<phys::object>(centralMass);
}

Orbit& Orbit::addPhysObject(const std::string& name, phys::object* physObject,
                            const linear_algebra::Vector& satellite_start_point,
                            const linear_algebra::Vector& satellite_start_speed) {
    m_physObjects[name].first = std::shared_ptr<phys::object>(physObject);

    // initialize start point
    m_physObjects[name].first->move(satellite_start_point);

    OrbitParameters& parameters = m_physObjects[name].second;
    //initialize mu
    parameters.mu = m_G * (m_centralMass->mass() + physObject->mass());
    Orbit_DBGout << "mu = " << m_physObjects[name].second.mu << std::endl;

    double distance = satellite_start_point.length();
    Orbit_DBGout << "distance = " << distance  << " m" << std::endl;

    double circle_speed = std::sqrt(m_G * m_centralMass->mass() / distance);
    Orbit_DBGout << "circle speed = " << circle_speed << " m/sec" << std::endl;

    double start_speed = satellite_start_speed.length();
    Orbit_DBGout << "start speed = " << start_speed << " m/sec" << std::endl;

    double a = 1 / (
            2 / distance -
            satellite_start_speed * satellite_start_speed / (m_G * m_centralMass->mass())
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
    parameters.bitangent = satellite_start_point.normalize();
    Orbit_DBGout << "e_r = " << parameters.bitangent << std::endl;

    // initialize e_t
    parameters.tangent = (start_speed > circle_speed ? 1 : -1) * satellite_start_speed.normalize();
    Orbit_DBGout << "e_t = " << parameters.tangent << std::endl;

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
                    {     parameters.norm[0],      parameters.norm[1],      parameters.norm[2], 0},
                    {  parameters.tangent[0],   parameters.tangent[1],   parameters.tangent[2], 0},
                    {parameters.bitangent[0], parameters.bitangent[1], parameters.bitangent[2], 0},
                    {                     0 ,                      0 ,                      0 , 1}
            });
}

Orbit& Orbit::removePhysObject(std::string& name) {
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

        double E = anomaly(obj.second.t - obj.second.time, obj.second.p, obj.second.e, obj.second.mu);

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
                        {obj.second.bitangent[0], obj.second.bitangent[1], obj.second.bitangent[2], 0},
                        {  obj.second.tangent[0],   obj.second.tangent[1],   obj.second.tangent[2], 0},
                        {     obj.second.norm[0],      obj.second.norm[1],      obj.second.norm[2], 0},
                        {                     0 ,                      0 ,                      0 , 1}
                }.T() * linear_algebra::Vector {
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
            Orbit_DBGout << "\te_r = " << e_r << std::endl;
            Orbit_DBGout << "\te_dr = " << e_dr << std::endl;
            obj.first->rotate(obj.second.norm, std::acos(c_angle) * 180 / M_PI);
            Orbit_DBGout << "\tOrientation = " << obj.first->orientation() << std::endl;
        }
    }
}

Orbit& Orbit::render(std::list<std::shared_ptr<glsl::object>>& draw_list) {
    draw_list.clear();
    double EARTH_R = 6371e+3;
    auto* center = new shape::solid::sphere(32, 32, 1);
    center->move(linear_algebra::Vector {
                    m_centralMass->position()[0], // x
                    m_centralMass->position()[2], // z
                    m_centralMass->position()[1]  // y
            })
            .scale(linear_algebra::Vector {
                    m_centralMass->scale()[0][0], // y
                    m_centralMass->scale()[2][2], // y
                    m_centralMass->scale()[1][1]  // x
            } / EARTH_R); // Earth's radius
    center->orientation(m_centralMass->orientation());
    center->update_color(helper::color(150, 50, 50));
    draw_list.push_back(std::shared_ptr<glsl::object>(center));

    for (auto& pr : m_physObjects) {
        auto& obj = pr.second.first;
        auto* draw_obj = new shape::solid::sphere(32, 32, 1);
        draw_obj->move(linear_algebra::Vector {
                    obj->position()[0], // x
                    obj->position()[2], // z
                    obj->position()[1]  // y
            } / EARTH_R * 2)
            .scale(linear_algebra::Vector {
                0.5, 0.5, 0.5
            })
            .orientation(obj->orientation());; // Earth's radius
        draw_obj->update_color(helper::color(100, 100, 100));
        draw_list.push_back(std::shared_ptr<glsl::object>(draw_obj));
    }
}
}
}