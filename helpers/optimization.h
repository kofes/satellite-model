#pragma once

#include <functional>
#include <list>

namespace helper {
    namespace optimization {
        typedef std::function<double(double, double, double)> Function;

        std::tuple<double, double, double> genetic(helper::optimization::Function function);

        std::tuple<double, double, double> gradient(helper::optimization::Function function);
    }
}