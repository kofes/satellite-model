#include <ctime>
#include <cmath>
#include "LinearAlgebra.h"
#include <map>
#include "optimization.h"

namespace helper {
double sigmoid(double scale, double res) {
    return 1 / (1 + std::exp(- 2 * scale * res));
}
namespace optimization {
linear_algebra::Vector gradient(Function fun, bool maximization, size_t countArgs,
                                const double MIN_VAL, const double MAX_VAL,
                                const linear_algebra::Vector& delta) {
    size_t countIterations = 0;

    size_t maxCountIterations = 1000;

    double epsilon = 1e-4;

    std::srand(std::time(nullptr));

    linear_algebra::Vector vec1(countArgs), vec2(countArgs), grad(countArgs);
    linear_algebra::Vector delta_i(countArgs, 0);
    double lambda = 0.01;

    do {
        for (size_t i = 0; i < countArgs; ++i) {
            delta_i[i] = delta[i];
            grad[i] = (fun(vec1 + delta_i) - fun(vec1 - delta_i)) / (2 * delta[i]);
            delta_i[i] = 0;
        }
        linear_algebra::Vector buff = vec2;
        vec2 = vec1 + (maximization ? 1 : -1) * lambda * grad;
        vec1 = buff;
        ++countIterations;
    } while (
            countIterations < maxCountIterations &&
            grad.length() > epsilon
            );

    return vec2;
}

linear_algebra::Vector full(Function fun, bool maximization,
                            size_t countArgs, const double MIN_VAL,
                            const double MAX_VAL,
                            const linear_algebra::Vector& delta) {
    linear_algebra::Vector result(countArgs, MIN_VAL);
    double resultValue = (maximization ? -1 : 1) * fun(result);

    linear_algebra::Vector buff = result;
    double value = resultValue;

    std::function<void(size_t)> recursion = [&] (size_t cycle) {
        if (cycle == countArgs) {
            value = (maximization ? -1 : 1) * fun(buff);
            if (resultValue > value) {
                resultValue = value;
                result = buff;
            }
            return;
        }
        for (size_t k = 0; k < (MAX_VAL - MIN_VAL) / delta[cycle]; ++k) {
            buff[cycle] = k * delta[cycle] + MIN_VAL;
            recursion(cycle+1);
        }
    };

    recursion(0);

    return result;
}

linear_algebra::Vector genetic(Function function,
                               bool maximization,
                               size_t countArgs,
                               const double MIN_VAL,
                               const double MAX_VAL,
                               const linear_algebra::Vector& delta) {
    size_t countIterations = 0;

    size_t initialPopulationSize = 70;
    size_t countSelections = 250;
    size_t maxSizePopulation = 100;
    size_t maxCountMutations = countSelections * 0.5;
    size_t maxCountIterations = 150;

    // significant value
    double maxSigma = 0.0001;

    std::srand(std::time(nullptr));

    double scaleFactor = 0.04;

    // create initial population
    double populationFitness = 0;

    std::multimap<double, linear_algebra::Vector> population;
    linear_algebra::Vector val(countArgs);

    for (size_t i = 0; i < initialPopulationSize; ++i) {
        for (size_t j = 0; j < countArgs; ++j)
            val[j] = std::rand() * 1. / RAND_MAX * (MAX_VAL - MIN_VAL) + MIN_VAL;

        double value = function(val);

        value *= (maximization ? -1 : 1);

        double fitness = sigmoid(scaleFactor, value);

        population.emplace(fitness, val);
        populationFitness += fitness;
    }

    // evaluate sigma
    double sigma = 0;
    for (auto& pr : population)
        sigma += std::pow(pr.first - populationFitness / population.size(), 2);
    sigma = std::sqrt(sigma / population.size());

    // start iterations
    while (countIterations <= maxCountIterations && sigma > maxSigma) {
        // crossover
        auto it = population.begin();
        for (size_t i = 0; i < countSelections; ++i) { //(parent with better parent)
            if (it == population.end()) {
                it = population.begin();
                ++it;
            } else if (it == population.begin()) {
                ++it; ++it;
            }

            if (std::rand() % 2)
                ++it;
            else
                --it;

            auto parent1 = *it;
            auto buff = it;
            if (it == population.end())
                --it; else ++it;
            auto parent2 = *it;
            it = buff;

            for (size_t j = 0; j < countArgs; ++j)
                val[j] = (std::rand() % 2) ? parent1.second[j] : parent2.second[j];

            double value = function(val);

            value *= (maximization ? -1 : 1);

            double fitness = sigmoid(scaleFactor, value);

            population.emplace(fitness, val);
            populationFitness += fitness;
        }

        // mutation
        for (size_t i = 0; i < maxCountMutations; ++i) {
            auto iter = population.rbegin();
            for (size_t j = 0; j < countArgs; ++j) {
                val[j] = (std::rand() % 2) ? iter->second[j] :
                         std::rand() * 1. / RAND_MAX * (MAX_VAL - MIN_VAL) + MIN_VAL;
            }

            double value = function(val);

            value *= (maximization ? -1 : 1);

            double fitness = sigmoid(scaleFactor, value);

            populationFitness -= iter->first;
            population.erase(std::next(iter).base());
            population.emplace(fitness, val);
            populationFitness += fitness;
        }

        // evaluate sigma
        sigma = 0;
        for (auto& pr : population)
            sigma += std::pow(pr.first - populationFitness / population.size(), 2);
        sigma = std::sqrt(sigma / population.size());

        std::vector<double> sigmaCutValues;
        double summarySigmaValues = 0;
        for (auto& pr : population) {
            sigmaCutValues.emplace_back(1 + (pr.first - populationFitness / population.size()) / (2 * sigma));
            summarySigmaValues += sigmaCutValues.back();
        }

        // selection
        while (population.size() > maxSizePopulation) {
            std::multimap<double, linear_algebra::Vector>::reverse_iterator iter = population.rbegin();
            populationFitness -= iter->first;
            population.erase(std::next(iter).base());
        }
        ++countIterations;
    }
    return population.begin()->second;
}

linear_algebra::Vector amoeba(Function function, bool maximization, size_t countArgs,
                              const double MIN_VAL, const double MAX_VAL,
                              const linear_algebra::Vector& delta) {
    size_t countIterations = 0;
    // significant values
    double alpha = 1,
            gamma = M_PI,
            rho = 0.5,
            sigma = 0.5;

    size_t countPoints = 150;
    size_t maxCountIterations = 2000;

    // significant value
    double maxSigma = 0.0001;

    std::srand(std::time(nullptr));

    // comparator
    auto comparator = [&] (
            const std::pair<double, linear_algebra::Vector>& pr1,
            const std::pair<double, linear_algebra::Vector>& pr2
    ) -> bool {
        return pr1.first <= pr2.first;
    };

    // create initial population & calculate centroid
    linear_algebra::Vector centerPoint(countArgs);
    std::vector<std::pair<double, linear_algebra::Vector>> population;
    linear_algebra::Vector val(countArgs);

    double populationFitness = 0;
    for (size_t i = 0; i < countPoints; ++i) {
        for (size_t j = 0; j < countArgs; ++j)
            val[j] = std::rand() * 1. / RAND_MAX * (MAX_VAL - MIN_VAL) + MIN_VAL;

        double value = function(val);

        value = (maximization ? -1 : 1) * value;

        population.emplace_back(value, val);
        populationFitness += value;
    }


    // evaluate sigma
    double var = 0;
    for (auto& pr : population)
        var += std::pow(pr.first - populationFitness / population.size(), 2);


    while (countIterations < maxCountIterations && std::sqrt(var / population.size()) > maxSigma) {
        // calculate centroid
        for (size_t i = 0; i < centerPoint.size(); ++i)
            centerPoint[i] = 0;
        for (auto& pr : population)
            centerPoint += pr.second;

        std::sort(population.begin(), population.end(), comparator);

        centerPoint -= population.rbegin()->second;
        centerPoint /= population.size() - 1;

        // evaluate sigma
        var = 0;
        for (auto& pr : population)
            var += std::pow(pr.first - populationFitness / population.size(), 2);

        // reflection
        linear_algebra::Vector reflectionPoint = centerPoint + alpha * (centerPoint - population.rbegin()->second);
        double reflectionValue = function(reflectionPoint);

        reflectionValue = (maximization ? -1 : 1) * reflectionValue;

        auto last = population.rbegin();
        last++;

        bool reflection = population.begin()->first <= reflectionValue < last->first;

        if (reflection) {
            populationFitness -= population.back().first;
            population.back() = std::make_pair(reflectionValue, reflectionPoint);
            populationFitness += population.back().first;
            ++countIterations;
            continue;
        }

        // expansion
        bool expansion = reflectionValue < population.begin()->first;

        if (expansion) {
            linear_algebra::Vector expandedPoint = centerPoint + gamma * (reflectionPoint - centerPoint);

            double expandedValue = function(expandedPoint);

            expandedValue = (maximization ? -1 : 1) * expandedValue;

            expansion = expandedValue < reflectionValue;

            populationFitness -= population.back().first;
            population.back() = (expansion) ?
                                std::make_pair(expandedValue, expandedPoint) :
                                std::make_pair(reflectionValue, reflectionPoint);

            populationFitness += population.back().first;

            ++countIterations;
            continue;
        }


        // contraction
        bool contraction = reflectionValue >= last->first;

        if (contraction) {
            linear_algebra::Vector contractionPoint = centerPoint + rho * (population.back().second - centerPoint);

            double contractionValue = function(contractionPoint);

            contractionValue = (maximization ? -1 : 1) * contractionValue;

            contraction = contractionValue < population.back().first;

            if (contraction) {
                populationFitness -= population.back().first;
                population.back() = std::make_pair(contractionValue, contractionPoint);
                populationFitness += population.back().first;

                ++countIterations;
                continue;
            }
        }

        // shrink
        auto it1 = population.begin();
        for (auto& pr : population) {
            pr.second = it1->second + sigma * (pr.second - it1->second);

            populationFitness -= pr.first;

            pr.first = function(pr.second);
            pr.first *= (maximization ? -1 : 1);

            populationFitness += pr.first;
        }
        ++countIterations;
    }

    return population.front().second;
}
}
}