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
    Orbit();

    Orbit& setCentralMass(phys::object* centralMass);

    Orbit& addPhysObject(const std::string& name, phys::object* physObject,
                         const helper::container::OrbitParameters& params);

    Orbit& removePhysObject(std::string& name);

    Orbit& update(double dt = 1 /*sec*/) override;

    Orbit& render(std::list<std::shared_ptr<glsl::object>>& draw_list);

private:

    void updateParameters(helper::container::OrbitParameters& params, double mass, double& r, double dt);

    std::shared_ptr<phys::object> m_centralMass;
    std::map<std::string, std::pair<std::shared_ptr<phys::object>, helper::container::OrbitParameters>> m_physObjects;

    const size_t TRACK_MAX_SIZE = 100;
    std::map<std::string, std::list<linear_algebra::Vector>> m_physObjectsTracks;

};
}
}
