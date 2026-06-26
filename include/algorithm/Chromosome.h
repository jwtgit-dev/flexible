#pragma once
#include <vector>
#include <memory>
#include <random>
#include "models/TimetableData.h"
#include "models/ScheduledClass.h"

namespace timetable {

struct Gene {
    int scheduledClassId;
    int timeSlotId;
    int classroomId;

    Gene() : scheduledClassId(-1), timeSlotId(-1), classroomId(-1) {}
    Gene(int scId, int tsId, int crId)
        : scheduledClassId(scId), timeSlotId(tsId), classroomId(crId) {}
};

class Chromosome {
public:
    Chromosome() = default;
    explicit Chromosome(int size) : genes_(size) {}

    const std::vector<Gene>& getGenes() const { return genes_; }
    std::vector<Gene>& getGenes() { return genes_; }
    int size() const { return static_cast<int>(genes_.size()); }

    const std::vector<double>& getObjectives() const { return objectives_; }
    void setObjectives(const std::vector<double>& obj) { objectives_ = obj; }
    void setObjective(int index, double value) { objectives_[index] = value; }

    int getRank() const { return rank_; }
    void setRank(int rank) { rank_ = rank; }

    double getCrowdingDistance() const { return crowdingDistance_; }
    void setCrowdingDistance(double dist) { crowdingDistance_ = dist; }

    bool isFeasible() const { return feasible_; }
    void setFeasible(bool f) { feasible_ = f; }

    double getConstraintViolation() const { return constraintViolation_; }
    void setConstraintViolation(double v) { constraintViolation_ = v; }

    Gene& operator[](int i) { return genes_[i]; }
    const Gene& operator[](int i) const { return genes_[i]; }

    bool dominates(const Chromosome& other) const;

private:
    std::vector<Gene> genes_;
    std::vector<double> objectives_;
    int rank_{0};
    double crowdingDistance_{0.0};
    bool feasible_{true};
    double constraintViolation_{0.0};
};

using ChromosomePtr = std::shared_ptr<Chromosome>;

class ChromosomeDecoder {
public:
    static std::vector<ScheduledClassPtr> decode(
        const Chromosome& chromosome,
        const TimetableDataPtr& data);
};

class ChromosomeInitializer {
public:
    ChromosomeInitializer(const TimetableDataPtr& data);

    Chromosome generateRandom(std::mt19937& rng) const;
    Chromosome generateGreedy(std::mt19937& rng) const;

private:
    TimetableDataPtr data_;
};

}
