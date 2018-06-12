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

// TEST: Runge-Kutta 4-th for zero function
TEST(IntegralsTest, ZeroFunctionRK4) {
    std::srand(std::time(nullptr));

    double dh = (std::rand() * 1.0 / RAND_MAX + 1);
    double start = std::rand() * 1.0 / RAND_MAX * 1000;
    double end = std::rand() * 1.0 / RAND_MAX * 1000;

    auto result = helper::integral::rk4([](const linear_algebra::Vector& val, double t) -> linear_algebra::Vector {
        return linear_algebra::Vector {0};
    }, linear_algebra::Vector {0}, start, end, dh);

    EXPECT_EQ(result[0], 0);
}


// TEST: Runge-Kutta 4-th for const function
TEST(IntegralsTest, ConstFunctionRK4) {
    std::srand(std::time(nullptr));


    size_t COUNT_ITERATIONS = 500;
    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        double dh = (std::rand() * 1.0 / RAND_MAX + 1);
        double start = std::rand() * 1.0 / RAND_MAX * 1000;
        double end = std::rand() * 1.0 / RAND_MAX * 1000;

        auto result = helper::integral::rk4([&](const linear_algebra::Vector& val, double t) -> linear_algebra::Vector {
            return linear_algebra::Vector {dh};
        }, linear_algebra::Vector {0}, -start, end, dh);


        if (std::fabs(result[0] - (start + end)*dh) > dh*dh*dh*dh)
            ++error_counter;
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
}

// TEST: Runge-Kutta 4-th for quadratic function
TEST(IntegralsTest, QuadFunctionRK4) {
    std::srand(std::time(nullptr));

    size_t COUNT_ITERATIONS = 500;
    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        size_t N = std::rand() % 200 + 200;
        double start = std::rand() * 1.0 / RAND_MAX * 10;
        double end = std::rand() * 1.0 / RAND_MAX * 10 + 10;

        double dh = (end - start) / N;

        auto result = helper::integral::rk4([&](const linear_algebra::Vector& val, double t) -> linear_algebra::Vector {
            return linear_algebra::Vector {2 * std::sqrt(val[0])};
        }, linear_algebra::Vector {start * start}, start, end, dh);

        if (std::fabs(result[0] - (end * end - start * start)) > dh*dh*dh)
            ++error_counter;
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
}

// TEST: Runge-Kutta–Fehlberg 4-th (5-th) for quadratic function
TEST(IntegralsTest, QuadFunctionRKF45) {
    std::srand(std::time(nullptr));

    size_t COUNT_ITERATIONS = 500;
    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        size_t N = std::rand() % 200 + 200;
        double start = std::rand() * 1.0 / RAND_MAX * 10;
        double end = std::rand() * 1.0 / RAND_MAX * 10 + 10;

        double dh = (end - start) / N;

        auto result = helper::integral::rkf45([&](const linear_algebra::Vector& val, double t) -> linear_algebra::Vector {
            return linear_algebra::Vector {2 * std::sqrt(val[0])};
        }, linear_algebra::Vector {start * start}, start, end, dh);

        if (std::fabs(result[0] - (end * end - start * start)) > dh*dh*dh*dh)
            ++error_counter;
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
}

// TEST: Runge-Kutta–Fehlberg 7-th (8-th) for quadratic function
TEST(IntegralsTest, QuadFunctionRKF78) {
    std::srand(std::time(nullptr));

    size_t COUNT_ITERATIONS = 500;
    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        size_t N = std::rand() % 200 + 200;
        double start = std::rand() * 1.0 / RAND_MAX * 10;
        double end = std::rand() * 1.0 / RAND_MAX * 10 + 10;

        double dh = (end - start) / N;

        auto result = helper::integral::rkf78([&](const linear_algebra::Vector& val, double t) -> linear_algebra::Vector {
            return linear_algebra::Vector {2 * std::sqrt(val[0])};
        }, linear_algebra::Vector {start * start}, start, end, dh);

        if (std::fabs(result[0] - (end * end - start * start)) > dh*dh*dh*dh)
            ++error_counter;
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
}
