#pragma once

#include <helpers/orbit/orbit.h>
#include <gtest/gtest.h>

#include <helpers/containers.h>

TEST(HelperOrbitTest, E) {
    std::srand(std::time(nullptr));

    helper::container::KeplerParameters params;
    while (std::fabs(params.e - 1) < 1e-4)
        params.e = 1. / (1 + std::rand() % 1000);

    double t = std::rand() * 1. / RAND_MAX * 1000;
    double epsilon = 1e-5;
    double mainMass = helper::constant::EARTH_MASS;
    double satMass = 30;

    double E = helper::orbit::E(params, mainMass, satMass, t, epsilon);

    double mu = helper::constant::G * (mainMass + satMass);
    double a = params.p / (1 - params.e * params.e);
    double n = std::sqrt(mu / (a*a*a));

    double delta = E - n * (t - params.tau);
    if (delta >= epsilon)
        EXPECT_LE(std::fabs(E + params.e * std::sin(E) - n * (t - params.tau)), epsilon);
}

TEST(HelperOrbitTest, Nu) {
    std::srand(std::time(nullptr));

    helper::container::KeplerParameters params;
    while (std::fabs(params.e - 1) < 1e-4)
        params.e = 1. / (1 + std::rand() % 1000);

    double t = std::rand() * 1. / RAND_MAX * 1000;
    double epsilon = 1e-5;
    double mainMass = helper::constant::EARTH_MASS;
    double satMass = 30;
    double E = helper::orbit::E(params, mainMass, satMass, t, epsilon);

    double nu = helper::orbit::nu(params, E);

    EXPECT_LE(std::fabs(std::cos(nu) - (std::cos(E) - params.e)/(1 - params.e * std::cos(E))), epsilon);
    EXPECT_LE(std::fabs(std::sin(nu) - std::sin(E)/(1 - params.e * std::cos(E)) * std::sqrt(1 - params.e * params.e)), epsilon);
}

TEST(HelperOrbitTest, RScalar) {
    std::srand(std::time(nullptr));

    helper::container::KeplerParameters params;
    while (std::fabs(params.e - 1) < 1e-4)
        params.e = 1. / (1 + std::rand() % 1000);

    double t = std::rand() * 1. / RAND_MAX * 1000;
    double epsilon = 1e-5;
    double mainMass = helper::constant::EARTH_MASS;
    double satMass = 30;
    double E = helper::orbit::E(params, mainMass, satMass, t, epsilon);

    double r = helper::orbit::r(params, E);

    double a = params.p / (1 - params.e * params.e);

    EXPECT_LE(std::fabs(r - a * (1 - params.e * std::sin(E))), epsilon);
}

TEST(HelperOrbitTest, T) {
    std::srand(std::time(nullptr));

    helper::container::KeplerParameters params;
    while (std::fabs(params.e - 1) < 1e-4)
        params.e = 1. / (1 + std::rand() % 1000);

    double epsilon = 1e-8;

    double mainMass = helper::constant::EARTH_MASS;
    double satMass = 30;

    double mu = helper::constant::G * (mainMass + satMass);
    double a = params.p / (1 - params.e * params.e);

    double T = helper::orbit::T(params, mainMass, satMass);

    EXPECT_LE(std::fabs(T - 2 * M_PI * std::sqrt(a*a*a/mu)), epsilon);
}
