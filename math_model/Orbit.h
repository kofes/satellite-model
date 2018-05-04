#pragma once

#include "LinearAlgebra.h"
#include "mvp/actions/actions.h"
#include <list>
#include <objects/objects.h>
#include <memory>
#include "helpers/helpers.h"


#define ORBIT_DBG

#ifdef ORBIT_DBG
#define Orbit_DBGout (std::cout << "Orbit DBG: ")
#define Orbit_out (std::cout)
#else
#define Orbit_DBGout (std::ostream(0))
#define OrbitDBG_out (std::ostream(0))
#endif

#define ORBIT_LOG

#ifdef ORBIT_LOG
#include <fstream>
static std::ofstream orbitLog("../orbit.txt");
#define Orbit_LOGout (orbitLog << "Orbit LOG: ")
#define OrbitLOG_out (orbitLog)
#else
#define Orbit_LOGout (std::ostream(nullptr))
#define Orbit_out (std::ostream(nullptr))
#endif

namespace math {
namespace model {
class Orbit: public phys::object {
public:

    struct OrbitParameters {
        double mu = 0;

        double T = 0, time = 0, t = 0; // sec
        double nu = 0; // rad
        linear_algebra::Vector norm, e_tau, e_r;


        double p = 0; // m
        double e = 0;

        double i = 0; // rad
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

    Orbit& updateKepler(double dt = 1);

    Orbit& render(std::list<std::shared_ptr<glsl::object>>& draw_list);

private:

    void updateParameters(OrbitParameters& params, double mass, double& r, double dt);

    double movement_integral(double dt, double nu, double e, double T) {
        double du = nu + dt/T;
        return (
                (1 + e) * du -
                        e / (6 * (1 + e)) * std::pow(du, 3) +
                        e * (1 - e + e * e) / (120 * std::pow(1 + e, 3)) * std::pow(du, 5)
        );
    }

    std::shared_ptr<phys::object> m_centralMass;
    std::map<std::string, std::pair<std::shared_ptr<phys::object>, OrbitParameters>> m_physObjects;
};
}
}
