#include "algorithm/NSGAII.h"
#include "algorithm/Chromosome.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <limits>

namespace timetable {

NSGAII::NSGAII(const TimetableDataPtr& data,
               const ConstraintManagerPtr& constraintManager,
               const NSGAIIParams& params)
    : data_(data),
      constraintManager_(constraintManager),
      params_(params),
      initializer_(data) {
    objectiveWeights_.resize(params_.numObjectives, 1.0);
}

std::vector<Chromosome> NSGAII::run() {
    auto population = initializePopulation();
    evaluatePopulation(population);

    auto initialFronts = fastNonDominatedSort(population);
    int idx = 0;
    for (auto& front : initialFronts) {
        crowdingDistanceAssignment(front);
        for (auto& chrom : front) {
            chrom.setRank(idx);
        }
        idx++;
    }
    std::vector<Chromosome> sortedPop;
    for (auto& front : initialFronts) {
        for (auto& chrom : front) {
            sortedPop.push_back(chrom);
        }
    }
    population = sortedPop;

    for (int gen = 0; gen < params_.maxGenerations; ++gen) {
        std::vector<Chromosome> offspring;
        offspring.reserve(params_.populationSize);

        while (static_cast<int>(offspring.size()) < params_.populationSize) {
            auto parent1 = tournamentSelection(population);
            auto parent2 = tournamentSelection(population);

            std::uniform_real_distribution<double> crossDist(0.0, 1.0);
            if (crossDist(rng_) < params_.crossoverRate) {
                auto child = crossover(parent1, parent2);
                offspring.push_back(child);
            } else {
                offspring.push_back(parent1);
            }
        }

        for (auto& chrom : offspring) {
            std::uniform_real_distribution<double> mutDist(0.0, 1.0);
            if (mutDist(rng_) < params_.mutationRate) {
                chrom = mutate(chrom);
            }
        }

        evaluatePopulation(offspring);

        population = elitistSelection(population, offspring);

        if (progressCallback_) {
            progressCallback_(gen + 1, population);
        }
    }

    auto fronts = fastNonDominatedSort(population);
    if (!fronts.empty()) {
        return fronts[0];
    }
    return {};
}

std::vector<Chromosome> NSGAII::initializePopulation() {
    std::vector<Chromosome> population;
    population.reserve(params_.populationSize);

    int greedyCount = params_.populationSize / 4;
    for (int i = 0; i < greedyCount; ++i) {
        population.push_back(initializer_.generateGreedy(rng_));
    }

    for (int i = greedyCount; i < params_.populationSize; ++i) {
        population.push_back(initializer_.generateRandom(rng_));
    }

    return population;
}

void NSGAII::evaluatePopulation(std::vector<Chromosome>& population) {
    for (auto& chrom : population) {
        evaluateChromosome(chrom);
    }
}

void NSGAII::evaluateChromosome(Chromosome& chrom) {
    auto schedule = ChromosomeDecoder::decode(chrom, data_);
    chrom.setObjectives(std::vector<double>(params_.numObjectives, 0.0));

    auto hardResult = constraintManager_->evaluateAllHard(data_, schedule);
    chrom.setFeasible(hardResult.satisfied);
    chrom.setConstraintViolation(static_cast<double>(hardResult.violationDetails.size()));

    double teacherPrefPenalty = 0.0;
    double intensityPenalty = 0.0;
    double distributionPenalty = 0.0;
    double compactnessPenalty = 0.0;
    double utilizationPenalty = 0.0;

    auto constraints = constraintManager_->getSoftConstraints();
    for (const auto& c : constraints) {
        auto result = c->evaluate(data_, schedule);
        if (c->getName() == "TeacherTimePreference") {
            teacherPrefPenalty = result.penalty;
        } else if (c->getName() == "TeacherTeachingIntensity") {
            intensityPenalty = result.penalty;
        } else if (c->getName() == "TeacherDistributionPreference") {
            distributionPenalty = result.penalty;
        } else if (c->getName() == "ClassScheduleCompactness") {
            compactnessPenalty = result.penalty;
        } else if (c->getName() == "ClassroomUtilizationBalance") {
            utilizationPenalty = result.penalty;
        }
    }

    double teacherSatisfaction = teacherPrefPenalty + intensityPenalty + distributionPenalty;

    if (params_.numObjectives >= 1) {
        chrom.setObjective(0, teacherSatisfaction);
    }
    if (params_.numObjectives >= 2) {
        chrom.setObjective(1, compactnessPenalty);
    }
    if (params_.numObjectives >= 3) {
        chrom.setObjective(2, utilizationPenalty);
    }

    if (!chrom.isFeasible()) {
        double baseViolation = chrom.getConstraintViolation() * 1000.0;
        for (int i = 0; i < params_.numObjectives; ++i) {
            chrom.setObjective(i, chrom.getObjectives()[i] + baseViolation);
        }
    }
}

std::vector<Chromosome> NSGAII::nonDominatedSort(std::vector<Chromosome> population) const {
    auto fronts = fastNonDominatedSort(population);
    std::vector<Chromosome> result;
    for (auto& front : fronts) {
        crowdingDistanceAssignment(front);
        for (auto& chrom : front) {
            result.push_back(chrom);
        }
    }
    return result;
}

std::vector<std::vector<Chromosome>>
NSGAII::fastNonDominatedSort(std::vector<Chromosome>& population) const {
    int n = static_cast<int>(population.size());
    std::vector<int> dominationCount(n, 0);
    std::vector<std::vector<int>> dominatedBy(n);
    std::vector<std::vector<int>> fronts(1);

    for (int p = 0; p < n; ++p) {
        for (int q = 0; q < n; ++q) {
            if (p == q) continue;
            if (population[p].dominates(population[q])) {
                dominatedBy[p].push_back(q);
            } else if (population[q].dominates(population[p])) {
                dominationCount[p]++;
            }
        }
        if (dominationCount[p] == 0) {
            population[p].setRank(0);
            fronts[0].push_back(p);
        }
    }

    int i = 0;
    while (!fronts[i].empty()) {
        std::vector<int> nextFront;
        for (int p : fronts[i]) {
            for (int q : dominatedBy[p]) {
                dominationCount[q]--;
                if (dominationCount[q] == 0) {
                    population[q].setRank(i + 1);
                    nextFront.push_back(q);
                }
            }
        }
        i++;
        if (!nextFront.empty()) {
            fronts.push_back(nextFront);
        } else {
            break;
        }
    }

    std::vector<std::vector<Chromosome>> chromFronts;
    for (const auto& front : fronts) {
        std::vector<Chromosome> chromFront;
        for (int idx : front) {
            chromFront.push_back(population[idx]);
        }
        chromFronts.push_back(chromFront);
    }
    return chromFronts;
}

void NSGAII::crowdingDistanceAssignment(std::vector<Chromosome>& front) const {
    if (front.empty()) return;
    int n = static_cast<int>(front.size());
    for (auto& chrom : front) {
        chrom.setCrowdingDistance(0.0);
    }

    for (int obj = 0; obj < params_.numObjectives; ++obj) {
        std::sort(front.begin(), front.end(),
                  [obj](const Chromosome& a, const Chromosome& b) {
                      return a.getObjectives()[obj] < b.getObjectives()[obj];
                  });

        front[0].setCrowdingDistance(std::numeric_limits<double>::infinity());
        front[n-1].setCrowdingDistance(std::numeric_limits<double>::infinity());

        double range = front[n-1].getObjectives()[obj] - front[0].getObjectives()[obj];
        if (range == 0.0) range = 1.0;

        for (int i = 1; i < n - 1; ++i) {
            double dist = (front[i+1].getObjectives()[obj] - front[i-1].getObjectives()[obj]) / range;
            front[i].setCrowdingDistance(front[i].getCrowdingDistance() + dist);
        }
    }
}

Chromosome NSGAII::tournamentSelection(const std::vector<Chromosome>& population) const {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(population.size()) - 1);

    Chromosome best = population[dist(rng_)];
    for (int i = 1; i < params_.tournamentSize; ++i) {
        Chromosome contender = population[dist(rng_)];

        if (contender.getRank() < best.getRank()) {
            best = contender;
        } else if (contender.getRank() == best.getRank()
                   && contender.getCrowdingDistance() > best.getCrowdingDistance()) {
            best = contender;
        }
    }
    return best;
}

Chromosome NSGAII::crossover(const Chromosome& parent1, const Chromosome& parent2) const {
    int size = parent1.size();
    Chromosome child = parent1;

    const auto& timeSlots = data_->getTimeSlots();
    const auto& classrooms = data_->getClassrooms();

    std::uniform_int_distribution<int> pointDist(0, size - 1);
    int start = pointDist(rng_);
    int end = pointDist(rng_);
    if (start > end) std::swap(start, end);

    for (int i = start; i <= end; ++i) {
        child[i] = parent2[i];
    }

    return child;
}

Chromosome NSGAII::mutate(const Chromosome& chrom) const {
    Chromosome mutated = chrom;
    int size = chrom.size();

    const auto& timeSlots = data_->getTimeSlots();
    const auto& classrooms = data_->getClassrooms();

    std::uniform_int_distribution<int> geneDist(0, size - 1);
    std::uniform_int_distribution<int> slotDist(0, static_cast<int>(timeSlots.size()) - 1);
    std::uniform_int_distribution<int> roomDist(0, static_cast<int>(classrooms.size()) - 1);
    std::uniform_int_distribution<int> typeDist(0, 2);

    int numMutations = std::max(1, size / 10);
    for (int i = 0; i < numMutations; ++i) {
        int geneIdx = geneDist(rng_);
        int mutationType = typeDist(rng_);

        Gene g = mutated[geneIdx];
        if (mutationType == 0) {
            g.timeSlotId = timeSlots[slotDist(rng_)]->getId();
        } else if (mutationType == 1) {
            g.classroomId = classrooms[roomDist(rng_)]->getId();
        } else {
            g.timeSlotId = timeSlots[slotDist(rng_)]->getId();
            g.classroomId = classrooms[roomDist(rng_)]->getId();
        }
        mutated[geneIdx] = g;
    }

    return mutated;
}

std::vector<Chromosome> NSGAII::elitistSelection(
        const std::vector<Chromosome>& parentPop,
        const std::vector<Chromosome>& childPop) const {
    std::vector<Chromosome> combined = parentPop;
    combined.insert(combined.end(), childPop.begin(), childPop.end());

    auto fronts = fastNonDominatedSort(combined);

    std::vector<Chromosome> newPopulation;
    newPopulation.reserve(params_.populationSize);

    int i = 0;
    while (i < static_cast<int>(fronts.size())
           && static_cast<int>(newPopulation.size() + fronts[i].size()) <= params_.populationSize) {
        crowdingDistanceAssignment(fronts[i]);
        for (auto& chrom : fronts[i]) {
            newPopulation.push_back(chrom);
        }
        i++;
    }

    if (static_cast<int>(newPopulation.size()) < params_.populationSize && i < static_cast<int>(fronts.size())) {
        crowdingDistanceAssignment(fronts[i]);
        std::sort(fronts[i].begin(), fronts[i].end(),
                  [](const Chromosome& a, const Chromosome& b) {
                      return a.getCrowdingDistance() > b.getCrowdingDistance();
                  });

        int remaining = params_.populationSize - static_cast<int>(newPopulation.size());
        for (int j = 0; j < remaining && j < static_cast<int>(fronts[i].size()); ++j) {
            newPopulation.push_back(fronts[i][j]);
        }
    }

    return newPopulation;
}

}
