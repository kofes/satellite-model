#pragma once

#include <functional>
#include <list>
#include <algorithm>
#include <map>
#include <LinearAlgebra.h>

namespace helper {
typedef std::function<double(const linear_algebra::Vector&)> Function;

double sigmoid(double scale, double res);

void bordering(linear_algebra::Vector& result,
    const linear_algebra::Vector& lower,
    const linear_algebra::Vector& upper
);

namespace optimization {
linear_algebra::Vector gradient(Function fun, bool maximization = false, size_t countArgs = 2,
                                const linear_algebra::Vector& minVals = linear_algebra::Vector(2, -M_PI),
                                const linear_algebra::Vector& maxVals = linear_algebra::Vector(2, M_PI),
                                const double delta = 0.0001, const double epsilon = 1e-5,
                                const size_t MAX_ITERATIONS = 1000);

linear_algebra::Vector full(Function fun, bool maximization = false, size_t countArgs = 2,
                                const linear_algebra::Vector& minVals = linear_algebra::Vector(2, -M_PI),
                                const linear_algebra::Vector& maxVals = linear_algebra::Vector(2, M_PI),
                                const double delta = 0.0001);

linear_algebra::Vector genetic(Function function, bool maximization = false, size_t countArgs = 2,
                               const linear_algebra::Vector& minVals = linear_algebra::Vector(2, -M_PI),
                               const linear_algebra::Vector& maxVals = linear_algebra::Vector(2, M_PI),
                               const double delta = 0.0001, const size_t MAX_ITERATIONS = 1000,
                               const size_t initialPopulationSize = 100,
                               const size_t populationSize = 30,
                               const size_t countCrossovers = 100,
                               const size_t countMutations = 250,
                               const double scaleFactor = 1);

linear_algebra::Vector amoeba(Function function, bool maximization = false, size_t countArgs = 2,
                              const linear_algebra::Vector& minVals = linear_algebra::Vector(2, -M_PI),
                              const linear_algebra::Vector& maxVals = linear_algebra::Vector(2, M_PI),
                              const size_t MAX_ITERATIONS = 1000);
}   // namespace optimization
}   // namespace helper
