#pragma once

#include "LinearAlgebra.h"
#include "mvp/actions/actions.h"
#include <list>
#include <geometric_object/geometric_object.h>
#include <objects/objects.h>
#include <memory>


#define ORBIT_DBG

#ifdef ORBIT_DBG
#define Orbit_DBGout (std::cout << "Orbit DBG: ")
#define Orbit_out (std::cout)
#else
#define Orbit_DBGout (std::ostream(0))
#define Orbit_out (std::ostream(0))
#endif

namespace math {
namespace model {
class Orbit: public phys::object {
public:

    struct OrbitParameters {
        double mu = 0;
        double p = 0; // m
        double e = 0;
        double T = 0, time = 0, t = 0; // sec
        double nu = 0; // rad
        double i = 0; // rad
        linear_algebra::Vector norm, e_tau, e_r;
        double Omega = 0;
        double omega = 0;
    };

    Orbit();

    Orbit& setCentralMass(phys::object* centralMass);

    Orbit& addPhysObject(const std::string& name, phys::object* physObject,
                         const linear_algebra::Vector& satellite_start_point,
                         const linear_algebra::Vector& satellite_start_speed);

    Orbit& addPhysObject(const std::string& name, phys::object* physObject,
                         const OrbitParameters& params);

    Orbit& removePhysObject(std::string& name);

    Orbit& update(double dt = 1 /*sec*/) override;

    Orbit& render(std::list<std::shared_ptr<glsl::object>>& draw_list);

private:
    double movement_integral(double dt, double nu, double e, double T) {
        double du = nu + dt/T;
        return (
                (1 + e) * du -
                        e / (6 * (1 + e)) * std::pow(du, 3) +
                        e * (1 - e + e * e) / (120 * std::pow(1 + e, 3)) * std::pow(du, 5)
        );
    }

    double anomaly(double dt, double p, double e, double mu) {
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

    double m_G = 6.67408e-11; // m^3 / (kg * sec^2)

    std::shared_ptr<phys::object> m_centralMass;
    std::map<std::string, std::pair<std::shared_ptr<phys::object>, OrbitParameters>> m_physObjects;
};
}
}
