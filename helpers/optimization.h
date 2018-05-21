#pragma once

#include <functional>
#include <list>
#include <algorithm>
#include <map>
#include <LinearAlgebra.h>

namespace helper {
typedef std::function<double(const linear_algebra::Vector&)> Function;

double sigmoid(double scale, double res);
namespace optimization {
    linear_algebra::Vector gradient(Function fun, bool maximization = false, size_t countArgs = 2,
                                    const double MIN_VAL = -M_PI, const double MAX_VAL =  M_PI,
                                    const linear_algebra::Vector& delta = linear_algebra::Vector(2, 0.0001));

    linear_algebra::Vector full(Function fun, bool maximization = false, size_t countArgs = 2,
                                const double MIN_VAL = -M_PI, const double MAX_VAL =  M_PI,
                                const linear_algebra::Vector& delta = linear_algebra::Vector(2, 0.0001));

    linear_algebra::Vector genetic(Function function, bool maximization = false, size_t countArgs = 2,
                                   const double MIN_VAL = -M_PI, const double MAX_VAL =  M_PI,
                                   const linear_algebra::Vector& delta = linear_algebra::Vector(2, 0.0001));

    linear_algebra::Vector amoeba(Function function, bool maximization = false, size_t countArgs = 2,
                                  const double MIN_VAL = -M_PI, const double MAX_VAL =  M_PI,
                                  const linear_algebra::Vector& delta = linear_algebra::Vector(2, 0.0001));
}
}