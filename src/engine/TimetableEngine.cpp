#include "engine/TimetableEngine.h"
#include "constraints/HardConstraints.h"
#include "constraints/SoftConstraints.h"
#include "algorithm/Chromosome.h"
#include <iostream>
#include <algorithm>

namespace timetable {

TimetableEngine::TimetableEngine(const TimetableDataPtr& data)
    : data_(data),
      constraintManager_(std::make_shared<ConstraintManager>()) {
    setupDefaultConstraints();
}

void TimetableEngine::setupDefaultConstraints() {
    constraintManager_->addConstraint(std::make_shared<NoTeacherConflictConstraint>());
    constraintManager_->addConstraint(std::make_shared<NoClassroomConflictConstraint>());
    constraintManager_->addConstraint(std::make_shared<NoClassGroupConflictConstraint>());
    constraintManager_->addConstraint(std::make_shared<ClassroomSuitabilityConstraint>());
    constraintManager_->addConstraint(std::make_shared<ClassroomCapacityConstraint>());
    constraintManager_->addConstraint(std::make_shared<TeacherUnavailableConstraint>());

    auto timePref = std::make_shared<TeacherTimePreferenceConstraint>();
    timePref->setWeight(params_.timePreferenceWeight);
    constraintManager_->addConstraint(timePref);

    auto intensityPref = std::make_shared<TeacherTeachingIntensityConstraint>();
    intensityPref->setWeight(params_.intensityPreferenceWeight);
    constraintManager_->addConstraint(intensityPref);

    auto distPref = std::make_shared<TeacherDistributionPreferenceConstraint>();
    distPref->setWeight(params_.distributionPreferenceWeight);
    constraintManager_->addConstraint(distPref);

    auto compactness = std::make_shared<ClassScheduleCompactnessConstraint>();
    compactness->setWeight(params_.compactnessWeight);
    constraintManager_->addConstraint(compactness);

    auto utilization = std::make_shared<ClassroomUtilizationBalanceConstraint>();
    utilization->setWeight(params_.utilizationWeight);
    constraintManager_->addConstraint(utilization);
}

std::vector<TimetableResult> TimetableEngine::solve() {
    NSGAIIParams nsgaParams;
    nsgaParams.populationSize = params_.populationSize;
    nsgaParams.maxGenerations = params_.maxGenerations;
    nsgaParams.crossoverRate = params_.crossoverRate;
    nsgaParams.mutationRate = params_.mutationRate;
    nsgaParams.numObjectives = params_.numObjectives;

    auto timePref = constraintManager_->getConstraint("TeacherTimePreference");
    if (timePref) timePref->setWeight(params_.timePreferenceWeight);

    auto intensityPref = constraintManager_->getConstraint("TeacherTeachingIntensity");
    if (intensityPref) intensityPref->setWeight(params_.intensityPreferenceWeight);

    auto distPref = constraintManager_->getConstraint("TeacherDistributionPreference");
    if (distPref) distPref->setWeight(params_.distributionPreferenceWeight);

    auto compactness = constraintManager_->getConstraint("ClassScheduleCompactness");
    if (compactness) compactness->setWeight(params_.compactnessWeight);

    auto utilization = constraintManager_->getConstraint("ClassroomUtilizationBalance");
    if (utilization) utilization->setWeight(params_.utilizationWeight);

    NSGAII algorithm(data_, constraintManager_, nsgaParams);
    if (progressCallback_) {
        algorithm.setProgressCallback(progressCallback_);
    }

    auto paretoChromosomes = algorithm.run();

    paretoFront_.clear();
    for (const auto& chrom : paretoChromosomes) {
        if (chrom.isFeasible()) {
            paretoFront_.push_back(chromosomeToResult(chrom));
        }
    }

    if (paretoFront_.empty()) {
        std::cerr << "Warning: No feasible solutions found in Pareto front." << std::endl;
    }

    return paretoFront_;
}

TimetableResult TimetableEngine::getBestResult() const {
    if (paretoFront_.empty()) {
        return TimetableResult{};
    }

    double bestScore = std::numeric_limits<double>::max();
    size_t bestIdx = 0;

    for (size_t i = 0; i < paretoFront_.size(); ++i) {
        double score = 0.0;
        for (size_t j = 0; j < paretoFront_[i].objectiveValues.size(); ++j) {
            score += paretoFront_[i].objectiveValues[j];
        }
        if (score < bestScore) {
            bestScore = score;
            bestIdx = i;
        }
    }

    return paretoFront_[bestIdx];
}

TimetableResult TimetableEngine::chromosomeToResult(const Chromosome& chrom) const {
    TimetableResult result;
    result.schedule = ChromosomeDecoder::decode(chrom, data_);
    result.feasible = chrom.isFeasible();
    result.objectiveValues = chrom.getObjectives();
    result.rank = chrom.getRank();
    result.crowdingDistance = chrom.getCrowdingDistance();

    result.totalSoftPenalty = constraintManager_->calculateTotalSoftPenalty(data_, result.schedule);

    auto constraints = constraintManager_->getSoftConstraints();
    for (const auto& c : constraints) {
        auto eval = c->evaluate(data_, result.schedule);
        result.constraintScores[c->getName()] = eval.penalty;
    }

    return result;
}

}
