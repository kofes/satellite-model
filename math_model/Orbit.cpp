#include "Orbit.h"

#include "Earth.h"
#include "Satellite.h"

#define ORBIT_DBG

#include <iostream>
#include <geometric_object/solids/solids.h>

#ifdef ORBIT_DBG
#define Orbit_DBGout (std::cout << "Orbit DBG: ")
#define Orbit_out (std::cout)
#else
#define Orbit_DBGout (std::ostream(0))
#define Orbit_out (std::ostream(0))
#endif

namespace math {
namespace model {
class Orbit::core {
public:
    core() = default;

    double G = 6.67408e-11; // m^3 / (kg * sec^2)
    double mu;
    double p; // m
    double e;
    double T, time, t; // sec
    double nu; // rad
    double i; // rad
    linear_algebra::Vector norm, tangent, bitangent;
    double Omega = 0;
    double omega = 0;

    double movement_integral(double dt) {
        double du = nu + dt/T;
        return (
                (1 + e) * du -
                e / (6 * (1 + e)) * std::pow(du, 3) +
                e * (1 - e + e * e) / (120 * std::pow(1 + e, 3)) * std::pow(du, 5)
        );
    }

    double anomaly(double dt) {
        double a = p / (1 - e * e);
        double M = std::sqrt(mu / std::pow(a, 3)) * dt;
        double E = M, E_0;

        Orbit_DBGout << "E_0 = " << E << std::endl;
        do {
            E_0 = E;
            E = e * std::sin(E_0) + M;
            Orbit_out << "\tE = " << E << std::endl;
        } while (std::fabs(E - E_0) > 1e-5);
        Orbit_DBGout << "E = " << E << std::endl;

        return E;
    }

    Earth earth;
    Satellite satellite;
};

Orbit::Orbit(
        const linear_algebra::Vector& satellite_start_point,
        const linear_algebra::Vector& satellite_start_speed
) : m_core(new Orbit::core) {
    // initialize Satellite position
    m_core->satellite.move(m_core->earth.position());
    m_core->satellite.move(satellite_start_point);

    // initialize mu
    m_core->mu = m_core->G * (m_core->earth.mass() + m_core->satellite.mass());
    Orbit_DBGout << "mu = " << m_core->mu << std::endl;

    double distance = satellite_start_point.length();
    Orbit_DBGout << "distance = " << distance  << " m" << std::endl;

    double circle_speed = std::sqrt(m_core->G * m_core->earth.mass() / distance);
    Orbit_DBGout << "circle speed = " << circle_speed << " m/sec" << std::endl;

    double start_speed = satellite_start_speed.length();
    Orbit_DBGout << "start speed = " << start_speed << " m/sec" << std::endl;

    double a = 1 / (
               2 / distance -
               satellite_start_speed * satellite_start_speed / (m_core->G * m_core->earth.mass())
    );
    Orbit_DBGout << "a = " << a << " m" << std::endl;

    /**
     * if start_speed >  circle_speed, then 'perigei' at start_point
     * if start_speed == circle_speed, then we have circle!!!
     * if start_speed <  circle_speed, then 'apogei' at start_point
     */

    // initialize normal
    m_core->norm = linear_algebra::vectorMultiply({satellite_start_point, satellite_start_speed}).normalize();
    Orbit_DBGout << "norm = " << m_core->norm << std::endl;

    // initialize e_r
    m_core->bitangent = satellite_start_point.normalize();
    Orbit_DBGout << "e_r = " << m_core->bitangent << std::endl;

    // initialize e_t
    m_core->tangent = (start_speed > circle_speed ? -1 : 1) * satellite_start_speed.normalize();
    Orbit_DBGout << "e_t = " << m_core->tangent << std::endl;

    // initialize i
    m_core->i = std::acos(m_core->norm * linear_algebra::Vector {0, 0, 1});
    Orbit_DBGout << "i = " << m_core->i << std::endl;

    double r_1 = 2 * a - distance;

    // initialize p
    m_core->p = 2 * r_1 * distance / (r_1 + distance);
    Orbit_DBGout << "p = " << m_core->p << " m" << std::endl;

    // initialize e
    m_core->e = std::fabs(r_1 - distance) / (r_1 + distance);
    Orbit_DBGout << "e = " << m_core->e << std::endl;

    // initialize period T
    m_core->T = 2 * M_PI * std::sqrt(a * a * a / m_core->mu);
    Orbit_DBGout << "T = " << m_core->T/3600 << " hour" << std::endl;

    // initialize perigei time
    m_core->time = m_core->T / 2 * (start_speed > circle_speed);
    Orbit_DBGout << "time = " << m_core->time/3600 << " hour" << std::endl;

    // initialize time
    m_core->t = 0;
    Orbit_DBGout << "time = " << m_core->t/3600 << " hour" << std::endl;

    // initialize start nu(t_0)
    m_core->nu = M_PI * (start_speed < circle_speed);
    Orbit_DBGout << "nu = " << m_core->nu << " rad" << std::endl;
}

Orbit& Orbit::move_satellite(double dt) {
    linear_algebra::Vector r = m_core->satellite.position() - m_core->earth.position();
    Orbit_DBGout << "r = " << r << " m" << std::endl;

    m_core->t = (m_core->t + dt) - (int)((m_core->t + dt) / m_core->T) * m_core->T;
    Orbit_DBGout << "time = " << m_core->t << std::endl;

    double E = m_core->anomaly(m_core->t - m_core->time);

    double a = m_core->p / (1 - m_core->e * m_core->e);
    double tmp_E = 1 - m_core->e * std::cos(E);

    double sclr_r = a * tmp_E;
    Orbit_DBGout << "r(E) = " << sclr_r << std::endl;

    double c_nu = (std::cos(E) - m_core->e)/tmp_E;
    Orbit_DBGout << "cos(nu(E)) = " << c_nu << std::endl;

    double s_nu = std::sin(E)/tmp_E * std::sqrt(1 - m_core->e * m_core->e);
    Orbit_DBGout << "sin(nu(E)) = " << s_nu << std::endl;
    Orbit_DBGout << "1 = " << s_nu*s_nu + c_nu * c_nu << std::endl;

    double I_2 = -1082.2e-6;

    m_core->Omega += 3 / 2. * I_2 * std::pow(m_core->earth.eR() / m_core->p, 2) * std::cos(m_core->i) * dt;
    m_core->Omega -= (int)(m_core->Omega / (2 * M_PI)) * 2 * M_PI;
    Orbit_DBGout << "Omega = " << m_core->Omega * 180 / M_PI << std::endl;

    m_core->omega += 3 / 4. * I_2 * std::pow(m_core->earth.eR() / m_core->p, 2) *
                    (1 - 5 * std::cos(m_core->i) * std::cos(m_core->i)) * dt;
    m_core->omega -= (int)(m_core->omega / (2 * M_PI)) * 2 * M_PI;
    Orbit_DBGout << "omega = " << m_core->omega * 180 / M_PI << std::endl;

    linear_algebra::Matrix M_Omega = mvp::action::rotate({0,0,1}, m_core->Omega * 180 / M_PI);
    Orbit_DBGout << "M_Omega =\n" << M_Omega << std::endl;

    linear_algebra::Matrix M_omega = mvp::action::rotate(m_core->norm, m_core->omega * 180 / M_PI);
    Orbit_DBGout << "M_omega =\n" << M_Omega << std::endl;

    linear_algebra::Vector dr =
//            M_Omega *
            M_omega *
            linear_algebra::Matrix {
                    {m_core->bitangent[0], m_core->bitangent[1], m_core->bitangent[2], 0},
                    {  m_core->tangent[0],   m_core->tangent[1],   m_core->tangent[2], 0},
                    {     m_core->norm[0],      m_core->norm[1],      m_core->norm[2], 0},
                    {                  0 ,                   0,                    0 , 1}
            }.T() *
            linear_algebra::Vector {sclr_r * c_nu, sclr_r * s_nu, 0, 1} - r;
    Orbit_DBGout << "dr = " << dr << std::endl;

    m_core->satellite.move(dr);
    Orbit_DBGout << "satellite pos = " << m_core->satellite.position() << std::endl;
}

std::list<std::shared_ptr<geometry::object>>& Orbit::render(std::list<std::shared_ptr<geometry::object>>& objects) {
    objects.clear();
    auto* earth = new
            geometry::solid::sphere(1, 32, 32);
    earth->translate(linear_algebra::Vector {
                    m_core->earth.position()[0], // x
                    m_core->earth.position()[2], // z
                    m_core->earth.position()[1]  // y
            })
            .scale(linear_algebra::Vector {
                    m_core->earth.eR(), // y
                    m_core->earth.pR(), // z
                    m_core->earth.eR()  // x
            } / m_core->earth.R())
            .color(0, 0, 255);
    auto* satellite = new geometry::solid::sphere(0.1, 32, 32);
    satellite->translate(linear_algebra::Vector {
                    m_core->satellite.position()[0], // x
                    m_core->satellite.position()[2], // z
                    m_core->satellite.position()[1]  // y
            } / m_core->earth.R()*2)
            .color(255, 255, 255);

    objects.push_back(std::shared_ptr<geometry::object>(earth));
    objects.push_back(std::shared_ptr<geometry::object>(satellite));

    return objects;
}
}
}