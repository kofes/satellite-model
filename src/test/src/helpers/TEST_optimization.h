#pragma once

#include <helpers/optimization/optimization.h>
#include <gtest/gtest.h>

// TEST: 'full' optimization for Sphere function
// N.B.: VERY SLOW TEST
// TEST(OptimizationTest, SphereFunctionFull) {
//     auto fun = [&] (const linear_algebra::Vector& vec) {
//         for (size_t i = 0; i < vec.size(); ++i)
//             result += vec[i] * vec[i];
//         return result;
//     };
//
//     size_t COUNT_ARGS = 3;
//     double DELTA_H = 0.01;
//
//     auto vec = helper::optimization::full(fun,
//         false,
//         COUNT_ARGS,
//         -1, 1,
//         DELTA_H);
//
//     for (size_t i = 0; i < vec.size(); ++i)
//         EXPECT_LE(vec[i], DELTA_H / 2);
// }

// TEST: 'full' optimization for Rastring function
// N.B.: VERY SLOW TEST
// TEST(OptimizationTest, RastringFunctionFull) {
//     auto fun = [&] (const linear_algebra::Vector& vec) {
//         double A = 10;
//         double result = A * vec.size();
//         for (size_t i = 0; i < vec.size(); ++i)
//             result += vec[i] * vec[i] - A * std::cos(2 * M_PI * vec[i]);
//         return result;
//     };
//
//     size_t COUNT_ARGS = 3;
//     double DELTA_H = 0.01;
//
//     auto vec = helper::optimization::full(fun,
//         false,
//         COUNT_ARGS,
//         -5.12, 5.12,
//         DELTA_H);
//
//     for (size_t i = 0; i < vec.size(); ++i)
//         EXPECT_LE(vec[i], DELTA_H / 2);
// }

// TEST: Gradient algorithm optimization for Sphere function
TEST(OptimizationTest, SphereFunctionGradient) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i] * vec[i];
        return result;
    };

    size_t COUNT_ARGS = 5;
    double DELTA_H = 0.01;

    auto vec = helper::optimization::gradient(fun,
        false,
        COUNT_ARGS,
        linear_algebra::Vector(COUNT_ARGS, -10),
        linear_algebra::Vector(COUNT_ARGS, 10),
        DELTA_H);

    for (size_t i = 0; i < vec.size(); ++i)
        EXPECT_LE(std::fabs(vec[i]), DELTA_H / 2);
}


// TEST: Gradient algorithm optimization for Rastrigin function
TEST(OptimizationTest, RastringFunctionGradient) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double A = 10;
        double result = A * vec.size();
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i] * vec[i] - A * std::cos(2 * M_PI * vec[i]);
        return result;
    };

    size_t COUNT_ARGS = 5;
    double DELTA_H = 0.01;

    auto vec = helper::optimization::gradient(fun,
        false,
        COUNT_ARGS,
        linear_algebra::Vector(COUNT_ARGS, -5.12),
        linear_algebra::Vector(COUNT_ARGS, 5.12),
        DELTA_H);

    for (size_t i = 0; i < vec.size(); ++i)
        EXPECT_LE(std::fabs(vec[i]), DELTA_H / 2);
}

// TEST: Gradient algorithm optimization for Rosenbrock function
// Global minimum = (1, ..., 1)
TEST(OptimizationTest, RosenbrockFunctionGradient) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        double a = 1;
        double b = 100;
        for (size_t i = 0; i < vec.size()-1; ++i)
            result += b * std::pow(vec[i+1] - vec[i] * vec[i], 2)
                    + std::pow(a - vec[i], 2);
        return result;
    };

    size_t COUNT_ARGS = 5;
    double DELTA_H = 0.01;

    auto vec = helper::optimization::gradient(fun,
        false,
        COUNT_ARGS,
        linear_algebra::Vector(COUNT_ARGS, -2),
        linear_algebra::Vector(COUNT_ARGS, 2),
        DELTA_H);

    for (size_t i = 0; i < vec.size(); ++i)
        EXPECT_GE(std::fabs(vec[i] - 1), DELTA_H / 2);
}

// TEST: Nelder-Mead algorithm optimization for Sphere function
TEST(OptimizationTest, SphereFunctionNelderMead) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i] * vec[i];
        return result;
    };

    size_t COUNT_ARGS = 6;
    double DELTA_H = 1e-5;

    auto vec = helper::optimization::amoeba(fun,
        false,
        COUNT_ARGS,
        linear_algebra::Vector(COUNT_ARGS, -10),
        linear_algebra::Vector(COUNT_ARGS, 10),
        150 + 0.2 / DELTA_H
    );

    for (size_t i = 0; i < vec.size(); ++i)
        EXPECT_LE(std::fabs(vec[i]), DELTA_H / 2);
}

// TEST: Nelder-Mead algorithm optimization for Rosenbrock function
TEST(OptimizationTest, RosenbrockFunctionNelderMead) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        double a = 1;
        double b = 100;
        for (size_t i = 0; i < vec.size()-1; ++i)
            result += b * std::pow(vec[i+1] - vec[i] * vec[i], 2)
                    + std::pow(a - vec[i], 2);
        return result;
    };

    size_t COUNT_ARGS = 6;
    double DELTA_H = 1e-5;

    auto vec = helper::optimization::amoeba(fun,
        false,
        COUNT_ARGS,
        linear_algebra::Vector(COUNT_ARGS, -10),
        linear_algebra::Vector(COUNT_ARGS, 10),
        150 + 0.2 / DELTA_H
    );

    for (size_t i = 0; i < vec.size(); ++i)
        EXPECT_LE(std::fabs(vec[i] - 1), DELTA_H / 2);
}

// TEST: Nelder-Mead algorithm optimization for linear function
TEST(OptimizationTest, LinearFunctionNelderMead) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i];
        return result;
    };

    size_t COUNT_ARGS = 2;
    double DELTA_H = 1e-5;

    auto vec = helper::optimization::amoeba(fun,
        false,
        COUNT_ARGS,
        linear_algebra::Vector(COUNT_ARGS, -10),
        linear_algebra::Vector(COUNT_ARGS, 10),
        150
    );

    for (size_t i = 0; i < vec.size(); ++i)
        EXPECT_LE(std::fabs(vec[i] + 10), DELTA_H / 2);
}


// // TEST: Genetic algorithm optimization for linear function
// TEST(OptimizationTest, LinearFunctionGenetic) {
//     auto fun = [&] (const linear_algebra::Vector& vec) {
//         double result = 0;
//         for (size_t i = 0; i < vec.size(); ++i)
//             result += vec[i];
//         return result;
//     };
//
//     size_t COUNT_ARGS = 6;
//     double DELTA_H = 1e-5;
//
//     auto vec = helper::optimization::genetic(fun,
//         false,
//         COUNT_ARGS,
//         -10, 10,
//         DELTA_H
//     );
//
//     for (size_t i = 0; i < vec.size(); ++i)
//         EXPECT_LE(std::fabs(vec[i]), DELTA_H / 2);
// }
