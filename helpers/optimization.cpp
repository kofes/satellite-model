#include <ctime>
#include <cmath>
#include "LinearAlgebra.h"
#include <map>
#include "optimization.h"

namespace helper {
namespace optimization {
std::tuple<double, double, double> genetic(helper::optimization::Function function) {
    size_t sizeInitialPopulation = 5;
    size_t maxSizePopulation = 10;
    size_t countSelectionPairs = 5;
    size_t maxCountIteration = 500;
    bool makeMutation = false;

    linear_algebra::Vector rotationWeights {0.1, 0.1, 0.1};
    double function_weight = 1;

    std::srand(std::time(nullptr));

    // initialize first population
    double summaryFitness = 0;
    std::multimap<double, std::tuple<double, double, double>> population;
    for (size_t i = 0; i < sizeInitialPopulation; ++i) {
        double yaw = std::rand() * 1.0 / RAND_MAX * 2 * M_PI - M_PI;
        double pitch = std::rand() * 1.0 / RAND_MAX * 2 * M_PI - M_PI;
        double roll = std::rand() * 1.0 / RAND_MAX * 2 * M_PI - M_PI;

        // compute fitness for each chromosome & for population at all
        double res =
                function(yaw, pitch, roll) * function_weight
                - rotationWeights[0] * yaw / (2 * M_PI)
                - rotationWeights[1] * pitch / (2 * M_PI)
                - rotationWeights[2] * roll / (2 * M_PI);
        res = std::exp(res);
        population.insert(std::make_pair(res, std::make_tuple(yaw, pitch, roll)));
        summaryFitness += res;
    }

    size_t countIteration = 0;
    while (countIteration < maxCountIteration && std::fabs(population.begin()->first - population.end()->first) > 4) {
        // selection
        std::list<std::pair<std::tuple<double, double, double>, std::tuple<double, double, double>>> selection;
        for (size_t i = 0; i < countSelectionPairs; ++i) {
            std::tuple<double, double, double> parent_1, parent_2;
            double rand_1 = std::rand() * 1. / RAND_MAX * summaryFitness;
            double rand_2 = std::rand() * 1. / RAND_MAX * summaryFitness;
            double accumulator = 0;
            for (auto &chromosome: population) {
                double fitness = chromosome.first;
                if ((std::fabs(rand_1 - accumulator - fitness / 2) - fitness / 2) < 1e-5)
                    parent_1 = chromosome.second;
                if ((std::fabs(rand_2 - accumulator - fitness / 2) - fitness / 2) < 1e-5)
                    parent_2 = chromosome.second;
                accumulator += fitness;
            }
            selection.emplace_back(std::make_pair(parent_1, parent_2));
        }

        // crossover
        double childrenSummaryFitness = 0;
        std::multimap<double, std::tuple<double, double, double>> children;
        for (auto &parents: selection) {
            double yaw = std::get<0>((std::rand() % 2) ? parents.first : parents.second);
            double pitch = std::get<1>((std::rand() % 2) ? parents.first : parents.second);
            double roll = std::get<2>((std::rand() % 2) ? parents.first : parents.second);
            double res =
                    function(yaw, pitch, roll) * function_weight
                    - rotationWeights[0] * yaw / (2 * M_PI)
                    - rotationWeights[1] * pitch / (2 * M_PI)
                    - rotationWeights[2] * roll / (2 * M_PI);
            res = std::exp(res);
            children.emplace(res, std::make_tuple(yaw, pitch, roll));
            childrenSummaryFitness += res;
        }

        // mutation
        /// TODO: realize mutation
        if (makeMutation && childrenSummaryFitness < summaryFitness);

        population.insert(children.begin(), children.end());

        // erase fitness chromosomes
        while (population.size() > maxSizePopulation)
            population.erase(population.begin());

        // increment iteration's counter
        ++countIteration;
    }

    return population.end()->second;
};

std::tuple<double, double, double> gradient(helper::optimization::Function function) {

};
}
}