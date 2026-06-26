#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include "algorithm/Chromosome.h"
#include "constraints/IConstraint.h"
#include "models/TimetableData.h"

namespace timetable {

struct NSGAIIParams {
    int populationSize{100};
    int maxGenerations{200};
    double crossoverRate{0.8};
    double mutationRate{0.1};
    int numObjectives{3};
    int tournamentSize{3};
};

enum ObjectiveFunction {
    TeacherSatisfaction = 0,
    ClassScheduleCompactness = 1,
    ClassroomUtilizationBalance = 2,
    TotalSoftPenalty = 3
};

class NSGAII {
public:
    NSGAII(const TimetableDataPtr& data,
           const ConstraintManagerPtr& constraintManager,
           const NSGAIIParams& params = NSGAIIParams());

    std::vector<Chromosome> run();

    void setObjectiveWeights(const std::vector<double>& weights) { objectiveWeights_ = weights; }
    const std::vector<double>& getObjectiveWeights() const { return objectiveWeights_; }

    using ProgressCallback = std::function<void(int generation, const std::vector<Chromosome>& population)>;
    void setProgressCallback(ProgressCallback cb) { progressCallback_ = cb; }

private:
    std::vector<Chromosome> initializePopulation();
    void evaluatePopulation(std::vector<Chromosome>& population);
    void evaluateChromosome(Chromosome& chrom);

    std::vector<Chromosome> nonDominatedSort(std::vector<Chromosome> population) const;
    void crowdingDistanceAssignment(std::vector<Chromosome>& front) const;

    Chromosome tournamentSelection(const std::vector<Chromosome>& population) const;

    Chromosome crossover(const Chromosome& parent1, const Chromosome& parent2) const;
    Chromosome mutate(const Chromosome& chrom) const;

    std::vector<Chromosome> elitistSelection(
        const std::vector<Chromosome>& parentPop,
        const std::vector<Chromosome>& childPop) const;

    std::vector<std::vector<Chromosome>>
    fastNonDominatedSort(std::vector<Chromosome>& population) const;

    TimetableDataPtr data_;
    ConstraintManagerPtr constraintManager_;
    NSGAIIParams params_;
    std::vector<double> objectiveWeights_;

    mutable std::mt19937 rng_{42};
    ChromosomeInitializer initializer_;

    ProgressCallback progressCallback_;
};

}
