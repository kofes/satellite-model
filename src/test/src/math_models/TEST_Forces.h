#pragma once

#include <math_models/Forces/Forces.h>
#include <helpers/helpers.h>
#include <gtest/gtest.h>

TEST(ForcesTEST, SolarPressureZeroArea) {
    helper::container::SailParameters params;
    params.Bf = 2./3; params.Bb = 2./3;
    params.ef = 1; params.eb = 0;
    params.rho = 0.9; params.s = 0.8;
    params.area = 0;
    params.norm = {1, 0, 0};

    linear_algebra::Vector solar_norm = {1, 0, 0};
    linear_algebra::Vector force = force::solar(params, solar_norm);
    EXPECT_LE(std::fabs(force.length()), 1e-5);
}

TEST(ForcesTEST, SolarPressureOrthogonalVectors) {
    helper::container::SailParameters params;
    params.Bf = 2./3; params.Bb = 2./3;
    params.ef = 1; params.eb = 0;
    params.rho = 0.9; params.s = 0.8;
    params.area = 200;
    params.norm = {0, 1, 0};

    linear_algebra::Vector solar_norm = {1, 0, 0};
    linear_algebra::Vector force = force::solar(params, solar_norm);
    EXPECT_LE(std::fabs(force.length()), 1e-5);
}
