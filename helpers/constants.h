#pragma once

namespace helper {
    namespace constant {
        // Gravity contstant
        static const double G = 6.67408e-11; // m^3 / (kg * sec^2)

        // Earth
        static const double EARTH_R = 6371e+3; // m
        static const double EARTH_E_R = 6378.1e+3; // m
        static const double EARTH_P_R = 6356.8e+3; // m
        static const double EARTH_MASS = 5.97237e+24; // kg

        // Earth's non-izomorphic const
        static const double J2 = 1082.2e-6;

        // Aerodynamic's constants
        static const double C_d = 2.2;
        static const double EARTH_OMEGA = 7.29e-5; // rad/sec

        // Solar pressure
        static const double SOLAR_PRESSURE = 4.56e-6; // H/m^2
        static const double ATMOSPHERIC_PRESSURE_0 = 101325; // Pa
        static const double TEMPERATURE_0 = 288.15; // K

        // Earth's orbital characteristics
        static const double EARTH_ECCENTRICITY = 0.0167086; // (e)
        static const double EARTH_SEMIMAJOR_AXIS = 149.6e+9; // (a) m
        static const double EARTH_INCILATION = 7.155 * M_PI / 180; // (i)
        static const double EARTH_LONGITUDE_ASCENDING_NODE = 174.9 * M_PI / 180; // (Omega)
        static const double EARTH_ARGUMENT_PERAPSIS = 288.1 * M_PI / 180; // (omega)
        static const double EARTH_ECLIPTIC = (23 + 26./60) * M_PI / 180; // (eps)
    }
}