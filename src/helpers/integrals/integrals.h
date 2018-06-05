#pragma once

#include <cstdlib>
#include <functional>
#include <cmath>

namespace helper {
namespace integral {
typedef std::function<double(double)> function;

double rect(function f, double from, double to, double dh);
double trapez(function f, double from, double to, double dh);
double simpson(function f, double from, double to, double dh);

}   // namespace integral
}   // namespace helper
