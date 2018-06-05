#pragma once

#include <helpers/integrals/integrals.h>
#include <gtest/gtest.h>

// TEST: check for zeroes with Rectangle-integral
TEST(IntegralsTest, ZeroFunctionRect) {
    std::srand(std::time(nullptr));

    double dh = (std::rand() * 1.0 / RAND_MAX + 1);
    double start = std::rand() * 1.0 / RAND_MAX * 1000;
    double end = std::rand() * 1.0 / RAND_MAX * 1000;

    double result = helper::integral::rect([](double val) -> double {
        return 0;
    }, start, end, dh);
    EXPECT_EQ(result, 0);
}

// TEST: check for zeroes with Trapez-integral
TEST(IntegralsTest, ZeroFunctionTrapez) {
    std::srand(std::time(nullptr));

    double dh = (std::rand() * 1.0 / RAND_MAX + 1);
    double start = std::rand() * 1.0 / RAND_MAX * 1000;
    double end = std::rand() * 1.0 / RAND_MAX * 1000;

    double result = helper::integral::trapez([](double val) -> double {
        return 0;
    }, start, end, dh);
    EXPECT_EQ(result, 0);
}

// TEST: check for zeroes with Simpson-integral
TEST(IntegralsTest, ZeroFunctionSimpson) {
    std::srand(std::time(nullptr));

    double dh = (std::rand() * 1.0 / RAND_MAX + 1);
    double start = std::rand() * 1.0 / RAND_MAX * 1000;
    double end = std::rand() * 1.0 / RAND_MAX * 1000;

    double result = helper::integral::simpson([](double val) -> double {
        return 0;
    }, start, end, dh);
    EXPECT_EQ(result, 0);
}

// TEST: check for const with Rectangle-integral
TEST(IntegralsTest, ConstFunctionRect) {
    std::srand(std::time(nullptr));

    double dh = 0.0153;
    double start = 1000;
    double end = 1000;
    double result = helper::integral::rect([&](double val) -> double {
        return dh;
    }, -start, end, dh);

    EXPECT_LE(std::fabs(result - (start + end)*dh), (start + end)*dh/500);
}

// TEST: check for const with Trapez-integral
TEST(IntegralsTest, ConstFunctionTrapez) {
    std::srand(std::time(nullptr));

    double dh = 0.0153;
    double start = 1000;
    double end = 1000;
    double result = helper::integral::trapez([&](double val) -> double {
        return dh;
    }, -start, end, dh);

    EXPECT_LE(std::fabs(result - (start + end)*dh), (start + end)*dh/1000);
}

// TEST: check for const with Simpson-integral
TEST(IntegralsTest, ConstFunctionSimpson) {
    std::srand(std::time(nullptr));

    double dh = 0.0153;
    double start = 1000;
    double end = 1000;
    double result = helper::integral::simpson([&](double val) -> double {
        return dh;
    }, -start, end, dh);

    EXPECT_LE(std::fabs(result - (start + end)*dh), (start + end)*dh/1000);
}
