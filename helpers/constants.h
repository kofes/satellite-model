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
        static const double SOLAR_PRESSURE = 4.6e-6;
    }
}