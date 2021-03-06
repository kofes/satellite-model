#include <ctime>
#include <cmath>
#include <set>
#include "optimization.h"

namespace helper {
double sigmoid(double scale, double res) {
    return 1 / (1 + std::exp(- 2 * scale * res));
}

void bordering(linear_algebra::Vector& result,
    const linear_algebra::Vector& lower,
    const linear_algebra::Vector& upper
) {
    for (size_t i = 0; i < result.size(); ++i)
        if (result[i] < lower[i])
            result[i] = lower[i];
        else if (result[i] > upper[i])
            result[i] = upper[i];
}

namespace optimization {
linear_algebra::Vector gradient(Function fun, bool maximization, size_t countArgs,
    const linear_algebra::Vector& minVals,
    const linear_algebra::Vector& maxVals,
    const double delta, const double epsilon,
    const size_t MAX_ITERATIONS
) {
    size_t countIterations = 0;

    std::srand(std::time(nullptr));
    double RV = std::rand() * 1. / RAND_MAX;
    double lambda = 0.01;

    linear_algebra::Vector prev(countArgs),
                           result(RV * (maxVals - minVals) + minVals),
                           grad(countArgs);

   linear_algebra::Vector delta_i(countArgs, 0);
    do {
        for (size_t i = 0; i < countArgs; ++i) {
            delta_i[i] = delta;
            grad[i] = (fun(result + delta_i) - fun(result - delta_i)) / (2 * delta_i[i]);
            delta_i[i] = 0;
        }
        prev = result;
        result += (maximization ? 1 : -1) * lambda * grad;
        bordering(result, minVals, maxVals);
        ++countIterations;
    } while (
            (!MAX_ITERATIONS || countIterations < MAX_ITERATIONS) &&
            (result - prev).length() > epsilon);

    return result;
}

linear_algebra::Vector full(Function fun, bool maximization, size_t countArgs,
                            const linear_algebra::Vector& minVals,
                            const linear_algebra::Vector& maxVals,
                            const double delta) {
    linear_algebra::Vector result(minVals);
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
        size_t N = (maxVals - minVals)[cycle] / delta;
        for (size_t k = 0; k < N; ++k) {
            buff[cycle] = k * delta + minVals[cycle];
            recursion(cycle+1);
        }
    };

    recursion(0);

    return result;
}

linear_algebra::Vector genetic(Function function, bool maximization,
                               size_t countArgs,
                               const linear_algebra::Vector& minVals,
                               const linear_algebra::Vector& maxVals,
                               const size_t MAX_ITERATIONS,
                               const size_t initialPopulationSize,
                               const size_t populationSize,
                               const size_t countCrossovers,
                               const size_t countMutations,
                               const double scaleFactor
                           ) {
    std::srand(std::time(nullptr));

    // comparator
    auto comparator = [] (
        const std::pair<double, linear_algebra::Vector>& pr1,
        const std::pair<double, linear_algebra::Vector>& pr2
    ) -> bool {
        return pr1.first <= pr2.first;
    };

    // create initial population & calculate population fitness
    std::vector<std::pair<double, linear_algebra::Vector>> population(initialPopulationSize);
    for (size_t i = 0; i < initialPopulationSize; ++i) {
       linear_algebra::Vector args(countArgs);
       for (size_t i = 0; i < args.size(); ++i)
            args[i] = std::rand() * 1. / RAND_MAX * (maxVals[i] - minVals[i]) + minVals[i];

       double value = function(args);
       value = (maximization ? -1 : 1) * value;
       double fitness = sigmoid(scaleFactor, value);

       population[i] = std::make_pair(fitness, args);
    }

    // start iterations
    size_t countIterations = 0;
    do {
        double populationFitness = 0;
        for (size_t i = 0; i < population.size(); ++i)
            populationFitness += population[i].first;

        // ordering
        std::sort(population.begin(), population.end(), comparator);

        // evaluate sigma
        double sigma = 0;
        for (auto& pr : population)
            sigma += std::pow(pr.first - populationFitness / (population.size()+1), 2);
        sigma = std::sqrt(sigma / (population.size()+1));

        // 'sigma-cut'
        std::vector<double> sigmaCutValues;
        double summarySigmaValues = 0;
        for (auto& pr : population) {
            sigmaCutValues.emplace_back(1 + (pr.first - populationFitness / (population.size()+1)) / (2 * sigma + 1));
            summarySigmaValues += sigmaCutValues.back();
        }

        // parents selection
        std::vector<std::pair<double, linear_algebra::Vector>> parents;
        std::set<size_t> selectedChroms;
        while (summarySigmaValues > 0 && selectedChroms.size() < population.size() && selectedChroms.size() < populationSize) {
            // std::cout << "summarySigmaValues: " << selectedChroms.size() << std::endl;
            double RV = std::rand() * 1. / RAND_MAX * summarySigmaValues;
            double sumSigmaValues = 0;
            for (size_t i = 0; i < populationSize; ++i) {
                if (selectedChroms.find(i) != selectedChroms.end())
                    continue;
                if (sumSigmaValues <= RV && RV <= sumSigmaValues + sigmaCutValues[i]) {
                    parents.emplace_back(population[i]);
                    summarySigmaValues -= sigmaCutValues[i];
                    selectedChroms.emplace(i);
                }
            }
        }

        // crossover to produce new children
        std::vector<std::pair<double, linear_algebra::Vector>> children(countCrossovers);
        for (size_t i = 0; i < countCrossovers; ++i) {
            size_t ind1 = std::rand() % parents.size();
            size_t ind2 = ind1 + (ind1 > 0 ? -1 : 1);

            linear_algebra::Vector args(countArgs);
            for (size_t j = 0; j < countArgs; ++j)
                args[j] = (std::rand() % 2) ? parents[ind1].second[j]
                                            : parents[ind2].second[j];

            double value = function(args);
            value *= (maximization ? -1 : 1);
            double fitness = sigmoid(scaleFactor, value);

            children[i].first = fitness;
            children[i].second = args;
        }

        // mutation of children
        for (size_t i = 0; i < countMutations; ++i) {
            size_t ind = std::rand() % children.size() / 2;
            linear_algebra::Vector args(countArgs);
            for (size_t j = 0; j < countArgs; ++j) {
                args[j] = (std::rand() % 2) ? children[ind].second[j] :
                         std::rand() * 1. / RAND_MAX * (maxVals[j] - minVals[j]) + minVals[j];
            }

            double value = function(args);
            value *= (maximization ? -1 : 1);

            double fitness = sigmoid(scaleFactor, value);

            children[ind].first = fitness;
            children[ind].second = args;
        }

        // ordering
        std::sort(children.begin(), children.end(), comparator);

        // generate new generation by the best of old population & children
        std::vector<std::pair<double, linear_algebra::Vector>> newPopulation(populationSize);
        size_t chInd = 0, oldInd = 0;

        for (size_t i = 0; i < populationSize; ++i)
            if (population[oldInd].first < children[chInd].first) {
                newPopulation[i].first = population[oldInd].first;
                newPopulation[i].second = population[oldInd].second;
                ++oldInd;
            } else {
                newPopulation[i].first = children[chInd].first;
                newPopulation[i].second = children[chInd].second;
                ++chInd;
            }

        population = newPopulation;

        ++countIterations;
        // std::cout << countIterations << std::endl;
    } while (countIterations <= MAX_ITERATIONS);

    return population.front().second;
}

linear_algebra::Vector amoeba(Function function, bool maximization, size_t countArgs,
                              const linear_algebra::Vector& minVals,
                              const linear_algebra::Vector& maxVals,
                              const size_t MAX_ITERATIONS) {
    size_t countIterations = 0;

    const double alpha = 1;
    const double beta = 1 + 2. / countArgs;
    const double gamma = 0.75 - 1. / (2 * countArgs);
    const double sigma = 1 - 1. / countArgs;

    std::srand(std::time(nullptr));

    // comparator
    auto comparator = [&] (
            const std::pair<double, linear_algebra::Vector>& pr1,
            const std::pair<double, linear_algebra::Vector>& pr2
    ) -> bool {
        return pr1.first <= pr2.first;
    };

    // create initial simplex
    std::vector<std::pair<double, linear_algebra::Vector>> points(countArgs + 1);

    for (size_t i = 0; i < countArgs + 1; ++i) {
        linear_algebra::Vector args(countArgs);
        for (size_t i = 0; i < countArgs; ++i)
            args[i] = std::rand() * 1. / RAND_MAX * (maxVals[i] - minVals[i]) + minVals[i];

        double value = function(args);
        value = (maximization ? -1 : 1) * value;

        points[i] = std::make_pair(value, args);
    }

    // // DEBUG
    // std::cout << "count args: " << countArgs << std::endl;
    // //

    do {
        // ordering
        std::sort(points.begin(), points.end(), comparator);

        // // DEBUG
        //     std::cout << "sorted" << std::endl;
        //     for (auto& pr: points)
        //         std::cout << "f(" << pr.second << ") -> " << pr.first << std::endl;
        // //

        // calculate centroid
        linear_algebra::Vector centroid(countArgs, 0);
        for (size_t i = 0; i < points.size() - 1; ++i)
            centroid += points[i].second;
        centroid /= points.size() - 1;

        // // DEBUG
        //     std::cout << "centroid: " << centroid << std::endl;
        // //

        // reflection
        linear_algebra::Vector reflectionPoint = centroid + alpha * (centroid - points.back().second);

        // bordering
        bordering(reflectionPoint, minVals, maxVals);

        double reflectionValue = function(reflectionPoint);
        reflectionValue = (maximization ? -1 : 1) * reflectionValue;

        // // DEBUG
        //     std::cout << "reflection(" << reflectionPoint
        //               << ") -> " << reflectionValue << std::endl;
        // //

        auto lastPr = points[points.size() - 2];

        bool reflection = points.front().first <= reflectionValue
                       && reflectionValue < lastPr.first;

        if (reflection) {
            points[points.size() - 1] = std::make_pair(reflectionValue, reflectionPoint);

            // // DEBUG
            //     std::cout << "reflection" << std::endl;
            //     std::cout << "point: " << points[points.size() - 1].second << std::endl;
            //     std::cout << "point: " << reflectionPoint << std::endl;
            //     std::cout << "iteration: " << countIterations << std::endl;
            // //

            ++countIterations;
            continue;
        }

        // expansion
        bool expansion = reflectionValue < points.front().first;

        if (expansion) {
            linear_algebra::Vector expandedPoint = centroid + beta * (reflectionPoint - centroid);

            // bordering
            bordering(expandedPoint, minVals, maxVals);

            double expandedValue = function(expandedPoint);
            expandedValue = (maximization ? -1 : 1) * expandedValue;

            expansion = expandedValue < reflectionValue;

            points.back() = (expansion) ?
                                std::make_pair(expandedValue, expandedPoint) :
                                std::make_pair(reflectionValue, reflectionPoint);

            // // DEBUG
            //     printf("expansion\n");
            //     printf("iteration: %d\n", countIterations);
            // //

            ++countIterations;
            continue;
        }


        // outside contraction
        bool outsideContraction = lastPr.first <= reflectionValue
                               && reflectionValue < points.back().first;

        if (outsideContraction) {
            linear_algebra::Vector OCPoint = centroid + gamma * (reflectionPoint - centroid);

            // bordering
            bordering(OCPoint, minVals, maxVals);

            double OCValue = function(OCPoint);
            OCValue = (maximization ? -1 : 1) * OCValue;

            outsideContraction = OCValue <= reflectionValue;

            if (outsideContraction) {
                points.back() = std::make_pair(OCValue, OCPoint);

                // // DEBUG
                //     printf("outside contraction\n");
                //     printf("iteration: %d\n", countIterations);
                // //

                ++countIterations;
                continue;
            }
        }
        // inside contraction
        bool insideContraction = reflectionValue >= points.back().first;

        if (insideContraction) {
            linear_algebra::Vector ICPoint = centroid - gamma * (reflectionPoint - centroid);

            // bordering
            bordering(ICPoint, minVals, maxVals);

            double ICValue = function(ICPoint);
            ICValue = (maximization ? -1 : 1) * ICValue;

            insideContraction = ICValue < points.back().first;

            if (insideContraction) {
                points.back() = std::make_pair(ICValue, ICPoint);

                // // DEBUG
                //     printf("inside contraction\n");
                //     printf("iteration: %d\n", countIterations);
                // //

                ++countIterations;
                continue;
            }
        }

        // shrink
        auto pr1 = points.front();
        for (auto& pr : points) {
            pr.second = pr1.second + sigma * (pr.second - pr1.second);

            pr.first = function(pr.second);
            pr.first *= (maximization ? -1 : 1);
        }
        ++countIterations;
        // // DEBUG
        //     printf("shrink\n");
        //     printf("iteration: %d\n", countIterations);
        // //
    } while (countIterations < MAX_ITERATIONS);

    return points.front().second;
}
}   // namespace optimization
}   // namespace helper
