#pragma once
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include "models/TimetableData.h"
#include "constraints/IConstraint.h"
#include "algorithm/NSGAII.h"

namespace timetable {

struct TimetableResult {
    std::vector<ScheduledClassPtr> schedule;
    bool feasible{false};
    double totalSoftPenalty{0.0};
    std::vector<double> objectiveValues;
    int rank{0};
    double crowdingDistance{0.0};
    std::unordered_map<std::string, double> constraintScores;
};

using TimetableResultPtr = std::shared_ptr<TimetableResult>;

struct SchedulingParams {
    int populationSize{80};
    int maxGenerations{150};
    double crossoverRate{0.85};
    double mutationRate{0.15};
    int numObjectives{3};
    int maxTimeSeconds{120};
    bool useGreedyInit{true};
    double timePreferenceWeight{1.5};
    double intensityPreferenceWeight{1.2};
    double distributionPreferenceWeight{1.0};
    double compactnessWeight{0.8};
    double utilizationWeight{0.6};
};

class TimetableEngine {
public:
    explicit TimetableEngine(const TimetableDataPtr& data);

    void setupDefaultConstraints();
    void setParams(const SchedulingParams& params) { params_ = params; }
    const SchedulingParams& getParams() const { return params_; }

    ConstraintManagerPtr getConstraintManager() const { return constraintManager_; }

    std::vector<TimetableResult> solve();
    TimetableResult getBestResult() const;
    const std::vector<TimetableResult>& getParetoFront() const { return paretoFront_; }

    void setProgressCallback(NSGAII::ProgressCallback cb) { progressCallback_ = cb; }

private:
    void initializeConstraints();
    TimetableResult chromosomeToResult(const Chromosome& chrom) const;

    TimetableDataPtr data_;
    ConstraintManagerPtr constraintManager_;
    SchedulingParams params_;
    std::vector<TimetableResult> paretoFront_;
    NSGAII::ProgressCallback progressCallback_;
};

}
