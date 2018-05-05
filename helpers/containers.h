#pragma once

namespace helper {
    namespace container {
        struct SailParameters {
            double Bf = 0, Bb = 0;
            double ef = 0, eb = 0;
            double rho = 0, s = 0;
            double area = 0;
            linear_algebra::Vector norm = {0, 0, 0, 1};
        };

        struct KeplerParameters {
            double p = 1;
            double mu = 1;
            double e = 1;
            double nu = 0;
            double omega = 0;
            double Omega = 0;
            double u = 0;
            double i = 0;
            double r = 1;
        };

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
    }
}