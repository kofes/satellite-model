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

// TEST: Gradient algorithm optimization for linear function
TEST(OptimizationTest, LinearFunctionGradient) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i];
        return result;
    };

    size_t COUNT_ARGS = 3;
    double DELTA_H = 0.01;

    size_t error_counter = 0;
    size_t COUNT_ITERATIONS = 500;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        auto vec = helper::optimization::gradient(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            DELTA_H * DELTA_H,
            DELTA_H,
            2000
        );
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i] + 10) > DELTA_H) {
                ++error_counter;
                break;
            }
    }
    // std::cout << "COUNT ERRORS: " << error_counter << std::endl;


    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
}

// TEST: Gradient algorithm optimization for Sphere function
TEST(OptimizationTest, SphereFunctionGradient) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i] * vec[i];
        return result;
    };

    size_t COUNT_ARGS = 3;
    double DELTA_H = 0.01;
    size_t COUNT_ITERATIONS = 500;

    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        auto vec = helper::optimization::gradient(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            DELTA_H);
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]) > DELTA_H) {
                ++error_counter;
                break;
            }
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
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

    size_t COUNT_ARGS = 3;
    double DELTA_H = 0.01;
    size_t COUNT_ITERATIONS = 500;

    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        auto vec = helper::optimization::gradient(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -2),
            linear_algebra::Vector(COUNT_ARGS, 2),
            DELTA_H);
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]-1) > DELTA_H) {
                ++error_counter;
                break;
            }
    }

    EXPECT_GE(error_counter * 1. / COUNT_ITERATIONS, 0.85);
}

// TEST: Nelder-Mead algorithm optimization for linear function
TEST(OptimizationTest, LinearFunctionNelderMead) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i];
        return result;
    };

    size_t COUNT_ARGS = 3;
    double DELTA_H = 0.01;
    size_t COUNT_ITERATIONS = 500;

    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        auto vec = helper::optimization::amoeba(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            150
        );
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]+10) > DELTA_H) {
                ++error_counter;
                break;
            }
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
}

// TEST: Nelder-Mead algorithm optimization for Sphere function
TEST(OptimizationTest, SphereFunctionNelderMead) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i] * vec[i];
        return result;
    };

    size_t COUNT_ARGS = 3;
    double DELTA_H = 0.01;
    size_t COUNT_ITERATIONS = 500;

    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        auto vec = helper::optimization::amoeba(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            150 + 0.2 / DELTA_H
        );
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]) > DELTA_H) {
                ++error_counter;
                break;
            }
    }
    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.05);
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

    size_t COUNT_ITERATIONS = 1000;
    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        size_t COUNT_ARGS = std::rand() % 10 + 1;
        double DELTA_H = 0.01;

        auto vec = helper::optimization::amoeba(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            5000
        );
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]-1) > DELTA_H) {
                ++error_counter;
                break;
            }
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.3);
}

// TEST: Nelder-Mead algorithm optimization for custom function
TEST(OptimizationTest, CustomFunctionNelderMead) {
    linear_algebra::Vector delta {
            1  , -5  , 9.2 , 0.4  , 2.3,
            7.5, -8.6, 4.76, -0.23, 3.89
    };
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double A = 10;
        double result = A * vec.size();
        for (size_t i = 0; i < vec.size(); ++i)
            result += (vec[i] - delta[i]) * (vec[i] - delta[i]);
        return result;
    };

    size_t COUNT_ITERATIONS = 1000;
    size_t error_counter = 0;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        size_t COUNT_ARGS = std::rand() % 10 + 1;
        double DELTA_H = 0.01;

        auto vec = helper::optimization::amoeba(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            500
        );
        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]-delta[i]) > DELTA_H) {
                ++error_counter;
                break;
            }
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.3);
}

// TEST: Genetic algorithm optimization for linear function
TEST(OptimizationTest, LinearFunctionGenetic) {
    auto fun = [&] (const linear_algebra::Vector& vec) {
        double result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i];
        return result;
    };

    size_t COUNT_ITERATIONS = 1000;
    size_t error_counter = 0;

    size_t COUNT_ARGS = 6;
    double DELTA_H = 1e-3;

    for (size_t i = 0; i < COUNT_ITERATIONS; ++i) {
        size_t COUNT_ARGS = std::rand() % 10 + 1;
        double DELTA_H = 0.1;

        auto vec = helper::optimization::genetic(fun,
            false,
            COUNT_ARGS,
            linear_algebra::Vector(COUNT_ARGS, -10),
            linear_algebra::Vector(COUNT_ARGS, 10),
            70, // count iterations
            60, // initial population size
            40, // common population size
            60, // count crossovers
            15, // count mutations
            1 // scale factor
        );

        for (size_t i = 0; i < vec.size(); ++i)
            if (std::fabs(vec[i]+10) > DELTA_H * COUNT_ARGS) {
                ++error_counter;
                break;
            }
    }

    EXPECT_LE(error_counter * 1. / COUNT_ITERATIONS, 0.2);
}
