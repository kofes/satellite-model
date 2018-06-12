#pragma once

#include <cstdlib>
#include <functional>
#include <cmath>
#include <LinearAlgebra.h>

namespace helper {
namespace integral {
typedef std::function<double(double)> function;
typedef std::function<linear_algebra::Vector(const linear_algebra::Vector&, double)> multifunction;

double rect(function f, double from, double to, double dh);
double trapez(function f, double from, double to, double dh);
double simpson(function f, double from, double to, double dh);

linear_algebra::Vector rk4(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
);

linear_algebra::Vector rk5(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
);

linear_algebra::Vector rkf45(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
);

const std::vector<std::vector<double>> RKF78_A_TABLE = {
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {2.0/27.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {1.0/36.0, 1.0/12.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {1.0/24.0, 0.0, 1.0/8.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {5.0/12.0, 0.0, -25.0/16.0, 25.0/16.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
     0.0, 0.0, },
    {1.0/20.0, 0.0, 0.0, 1.0/4.0, 1.0/5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
     0.0},
    {-25.0/108.0, 0.0, 0.0, 125.0/108.0, -65.0/27.0, 125.0/54.0, 0.0,
     0.0, 0.0, 0.0, 0.0, 0.0},
    {31.0/300, 0.0, 0.0, 0.0, 61.0/225.0, -2.0/9.0, 13.0/900.0, 0.0,
     0.0, 0.0, 0.0, 0.0},
    {2.0, 0.0, 0.0, -53.0/6.0, 704.0/45.0, -107.0/9.0, 67.0/90.0, 3.0,
     0.0, 0.0, 0.0, 0.0},
    {-91.0/108.0, 0.0, 0.0, 23.0/108.0, -976.0/135.0, 311.0/54.0, -19.0/60.0,
     17.0/6.0, -1.0/12.0, 0.0, 0.0, 0.0},
    {2383.0/4100, 0.0, 0.0, -341.0/164.0, 4496.0/1025.0, -301.0/82.0,
     2133.0/4100.0, 45.0/82.0, 45.0/164.0, 18.0/41.0, 0.0, 0.0},
    {3.0/205.0, 0.0, 0.0, 0.0, 0.0, -6.0/41.0, -3.0/205.0, -3.0/41.0,
     3.0/41.0, 6.0/41.0, 0.0, 0.0},
    {-1777.0/4100.0, 0.0, 0.0, -341.0/164.0, 4496.0/1025.0, -289.0/82.0,
     2193.0/4100.0, 51.0/82.0, 33.0/164.0, 12.0/41.0, 0.0, 1.0}
};

const std::vector<double> RKF78_C_TABLE = {
    0.0, 2.0/27.0, 1.0/9.0, 1.0/6.0, 5.0/12.0, 0.5,
    5.0/6.0, 1.0/6.0, 2.0/3.0, 1.0/3.0, 1.0, 0.0, 1.0
};

const std::vector<double> RKF78_B_TABLE = {
    0.0,        // 0
    0.0,        // 1
    0.0,        // 2
    0.0,        // 3
    0.0,        // 4
    34.0/105.0, // 5
    9.0/35.0,   // 6
    9.0/35.0,   // 7
    9.0/280.0,  // 8
    9.0/280.0,  // 9
    0,          // 10
    41.0/840.0, // 11
    41.0/840.0  // 12
};

linear_algebra::Vector rkf78(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
);

}   // namespace integral
}   // namespace helper
